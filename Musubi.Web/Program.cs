using System.Diagnostics;
using System.Threading.RateLimiting;
using Microsoft.AspNetCore.RateLimiting;
using Microsoft.Extensions.FileProviders;
using Musubi.Compiler;

namespace Musubi.Web;

record RunRequest(string Source);

public static class Program
{
    private static void Main(string[] args)
    {
        Console.WriteLine(
            "Homedir: " + Environment.GetFolderPath(Environment.SpecialFolder.UserProfile)
        );
        WebApplicationBuilder builder = WebApplication.CreateBuilder(args);

        builder.WebHost.UseUrls("http://0.0.0.0:5103");

        // Add services to the container.
        // Learn more about configuring OpenAPI at https://aka.ms/aspnet/openapi
        builder.Services.AddOpenApi();
        builder.Services.AddEndpointsApiExplorer();
        builder.Services.AddSwaggerGen();

        builder.Services.AddRateLimiter(options =>
        {
            options.RejectionStatusCode = StatusCodes.Status429TooManyRequests;
            options.AddPolicy(
                "run",
                httpContext =>
                    RateLimitPartition.GetSlidingWindowLimiter(
                        httpContext.Connection.RemoteIpAddress?.ToString() ?? "unknown",
                        _ => new SlidingWindowRateLimiterOptions
                        {
                            PermitLimit = 10,
                            SegmentsPerWindow = 6,
                            Window = TimeSpan.FromMinutes(1),
                        }
                    )
            );

            options.OnRejected = (context, _) =>
            {
                if (context.Lease.TryGetMetadata(MetadataName.RetryAfter, out TimeSpan retryAfter))
                {
                    context.HttpContext.Response.Headers.RetryAfter = (
                        (int)retryAfter.TotalSeconds
                    ).ToString();
                }
                return ValueTask.CompletedTask;
            };
        });

        builder.Services.AddCors(options =>
        {
            if (builder.Environment.IsDevelopment())
            {
                options.AddPolicy(
                    "MusubiCors",
                    policy =>
                    {
                        policy
                            .WithOrigins("http://localhost:5103")
                            .AllowAnyHeader()
                            .AllowAnyMethod();
                    }
                );
            }
            else
            {
                options.AddPolicy(
                    "MusubiCors",
                    policy =>
                    {
                        policy
                            .WithOrigins("https://musubi.schlafhase.uk")
                            .WithMethods("POST")
                            .WithHeaders("Content-Type");
                    }
                );
            }
        });

        WebApplication app = builder.Build();

        app.UseCors("MusubiCors");
        app.UseRateLimiter();

        // Configure the HTTP request pipeline.
        if (app.Environment.IsDevelopment())
        {
            app.MapOpenApi();
        }

        app.UseHttpsRedirection();

        _ = app.MapPost(
                "/run",
                async (RunRequest req) =>
                {
                    Guid jobId = Guid.NewGuid();
                    string source = req.Source + "\n";
                    TextWriter cout = Console.Out;
                    StringWriter sw = new();
                    Console.SetOut(sw);

                    try
                    {
                        string c = MusubiCompiler.Compile(source, "Musubi Browser project");
                        File.WriteAllText("/tmp/" + jobId.ToString() + ".c", c);

                        ProcessStartInfo ccPsi = new("gcc", $"/tmp/{jobId}.c -o /tmp/{jobId}.out")
                        {
                            CreateNoWindow = true,
                        };

                        using Process ccProcess = new() { StartInfo = ccPsi };
                        ccProcess.Start();
                        ccProcess.WaitForExit();

                        ProcessStartInfo programPsi = new("/tmp/" + jobId + ".out")
                        {
                            CreateNoWindow = true,
                            RedirectStandardOutput = true,
                        };
                        using Process p = new() { StartInfo = programPsi };
                        p.Start();
                        p.WaitForExit();

                        if (p.ExitCode != 0)
                        {
                            Console.WriteLine(
                                "The process crashed. This is most likely because the output of the program wasn't in the correct format (scott list of integers representing a string). This can also be caused by a stack overflow."
                            );
                        }
                        sw.Write(p.StandardOutput.ReadToEnd());
                    }
                    catch (Exception e)
                    {
                        return Results.InternalServerError(sw + "\n" + e.Message);
                    }
                    finally
                    {
                        File.Delete("/tmp/" + jobId.ToString() + ".c");
                        File.Delete("/tmp/" + jobId.ToString() + ".out");
                        Console.SetOut(cout);
                    }
                    return Results.Ok(sw.ToString());
                }
            )
            .RequireRateLimiting("run");

        if (app.Environment.IsDevelopment())
        {
            app.UseSwagger();
            app.UseSwaggerUI();
        }

        PhysicalFileProvider fileProvider = new(Path.Combine(app.Environment.ContentRootPath, "wwwroot"));

        app.UseDefaultFiles(new DefaultFilesOptions { FileProvider = fileProvider });
        app.UseStaticFiles(new StaticFileOptions { FileProvider = fileProvider });

        app.Run();
    }
}

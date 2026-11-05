using Musubi.Compiler;

namespace Musubi.Tests;

public class Tests
{
    private Errors _errors;

    [SetUp]
    public void Setup()
    {
        _errors = new();
    }

    [Test]
    public void Test1()
    {
        Assert.Pass();
    }
}
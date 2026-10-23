namespace Musubi.Compiler
{
    public static class FileLookup
    {
        public static readonly List<string> MusubiLibraryDirs =
        [
            Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.UserProfile),
                ".musubi/"
            ),
        ];

        public static string GetPath(string path)
        {
            Console.WriteLine("Path requested: " + path);
            if (path.StartsWith("~/"))
            {
                string result = Path.Combine(
                    Environment.GetFolderPath(Environment.SpecialFolder.UserProfile),
                    path[2..]
                );
                Console.WriteLine("Resolved to " + result);
                return result;
            }
            else if (Path.IsPathFullyQualified(path))
            {
                Console.WriteLine("Resolved to " + path);
                return path;
            }

            foreach (string dir in MusubiLibraryDirs.Where(Path.IsPathFullyQualified))
            {
                string ret = getPath(path, dir);
                if (File.Exists(ret))
                {
                    Console.WriteLine("Resolved to " + ret);
                    return ret;
                }
            }
            throw new FileNotFoundException(
                "The requested module couldn't be found in any of the libarary directories."
            );
        }

        private static string getPath(string path, string relativeTo)
        {
            path = Path.GetFullPath(path, relativeTo);
            if (Directory.Exists(path))
            {
                path = Path.Combine(path, "module.mbim");
            }
            return path;
        }
    }
}

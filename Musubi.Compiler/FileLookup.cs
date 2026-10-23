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
            if (path.StartsWith("~/"))
            {
                return Path.Combine(
                    Environment.GetFolderPath(Environment.SpecialFolder.UserProfile),
                    path[2..]
                );
            }
            else if (Path.IsPathFullyQualified(path))
            {
                return path;
            }

            foreach (string dir in MusubiLibraryDirs.Where(Path.IsPathFullyQualified))
            {
                string ret = getPath(path, dir);
                if (File.Exists(ret))
                {
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

namespace Musubi.Compiler.Scanning
{
    // for #define, #include and #once (they will get expanded before reaching the parser so i have to store them in trivia)
    public class Diretive :Trivia { 
        public required string Value;
    }
}

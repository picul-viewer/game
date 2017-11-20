using System;
using System.IO;
using System.Xml;
using System.Collections.Generic;
using System.Diagnostics;

namespace shader_compiler
{
    class Program
    {
        struct Configuration
        {
            public String bits;
            public Int32 vertex_type;

            public Configuration(String bits)
            {
                this.bits = bits;
                vertex_type = 0;
            }
        }

        struct Shader
        {
            public String name;
            public String path;
            public String type;
            public List<String> defines;
            public List<Configuration> configurations;

            public Shader(String name, String path, String type)
            {
                this.name = name;
                this.path = path;
                this.type = type;
                defines = new List<String>();
                configurations = new List<Configuration>();
            }
        }
        
        enum Mode
        {
            Debug,
            Release
        }

        static void Main(string[] args)
        {
            Console.WriteLine("Shader compiler is preparing...");

            String config_path = (args.GetLength(0) == 0) ? "config.txt" : args[0];

            String fxc_path;
            Mode mode;
            String version;
            String sources_path;

            {
                TextReader config_file = new StreamReader(config_path);
                if(config_file == null)
                {
                    Console.WriteLine("Config file cannot be read!");
                    Console.ReadKey();
                    return;
                }

                fxc_path = config_file.ReadLine();

#if DEBUG
                mode = Mode.Debug;
#else
                mode = Mode.Release;
#endif

                version = config_file.ReadLine();
                sources_path = config_file.ReadLine();

                config_file.Close();
            }

            Console.WriteLine("Shader compiler finished preparing!");
            
            Console.WriteLine("Shader compiler is compiling shaders...");

            Console.Write("\r0%");
            
            String work_dir = Environment.CurrentDirectory + "\\resources\\shaders\\";

            List<KeyValuePair<String, String>> missing_list = new List<KeyValuePair<String, String>>();

            {
                TextReader missing_list_file = new StreamReader("../Debug/mising_shaders.txt");

                String shader_name = missing_list_file.ReadLine();
                String shader_config = missing_list_file.ReadLine();

                while ( ( shader_name.Length != 0 ) && ( shader_config.Length != 0 ) )
                {
                    missing_list.Add( new KeyValuePair<String, String>( shader_name, shader_config ) );

                    shader_name = missing_list_file.ReadLine();
                    shader_config = missing_list_file.ReadLine();
                }
            }

            Int32 failed_count = 0;

            for (Int32 index = 0; index < missing_list.Count; ++index)
            {
                String shader_name = missing_list[index].Key;
                String shader_config = missing_list[index].Value;

                String shader_type = shader_name.Substring(shader_name.Length - 2);

                Directory.CreateDirectory(work_dir + shader_name);
                    
                ProcessStartInfo info = new ProcessStartInfo();
                info.FileName = fxc_path;
                info.WindowStyle = ProcessWindowStyle.Hidden;

                // Version
                info.Arguments = "/T " + shader_type + "_" + version + " ";
                // Filename
                info.Arguments += "\"" + sources_path + @"\" + shader_name + "\"";

                info.Arguments += " /nologo /E \"main\"";
                info.Arguments += (mode == Mode.Debug) ? " /Zi /Od" : "";
                
                for (Int32 i = 0; i < 32; ++i)
                    if (shader_config[i] == '1')
                        info.Arguments += " /D \"CONFIG_BIT" + i.ToString( ) + "\"";
                
                String output_path = work_dir + shader_name + @"\" + shader_config;
                String errors_path = work_dir + shader_name + @"\" + shader_config + ".errors";

                info.Arguments += " /Fe \"" + errors_path + "\"";
                info.Arguments += " /Fo \"" + output_path + "\"";

                Process fxc_process = Process.Start(info);
                fxc_process.WaitForExit();
                
                FileStream errors = File.Open(errors_path, FileMode.Open);
                if (errors.Length != 0)
                {
                    ++failed_count;
                    errors.Close();
                }
                else
                {
                    errors.Close();
                    File.Delete(errors_path);
                }

                Console.Write("\r{0}%", (Int32)(index / (Single)missing_list.Count * 100));
            }

            Console.WriteLine();

            if (failed_count != 0)
            {
                Console.WriteLine("{0}% shaders were not successfully compiled!", (Int32)(failed_count / (Single)missing_list.Count * 100));
                Console.ReadKey();
                return;
            }

            Console.WriteLine("Shader compiler finished compiling shaders!");
            
            Console.WriteLine("SUCCESS!");
            Console.ReadKey();
            return;
        }
    }
}

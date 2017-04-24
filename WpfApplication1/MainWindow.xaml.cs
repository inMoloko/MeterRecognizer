using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Runtime.InteropServices;

namespace WpfApplication1
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        //[DllImport("C:\\Users\\gudim\\documents\\visual studio 2015\\Projects\\TestOpenCV\\x64\\Debug\\MMeter.dll")]
        [DllImport("MMeter.dll")]
        private static extern IntPtr getMeterValue(string imagePath, string workPath);
        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            try
            {
                var ptr = getMeterValue(@"c:\!vd\006.jpg", @"c:\!vd\work\");
                Title = Marshal.PtrToStringAnsi(ptr);
            }
            catch (Exception exc)
            {
                tbResult.Text = exc.ToString();
            }
        }

        public MainWindow()
        {
            InitializeComponent();
            Loaded += MainWindow_Loaded;
        }



        private static string PtrToStringUtf8(IntPtr ptr) // aPtr is nul-terminated
        {
            if (ptr == IntPtr.Zero)
                return "";
            int len = 0;
            while (System.Runtime.InteropServices.Marshal.ReadByte(ptr, len) != 0)
                len++;
            if (len == 0)
                return "";
            byte[] array = new byte[len];
            System.Runtime.InteropServices.Marshal.Copy(ptr, array, 0, len);
            return System.Text.Encoding.UTF8.GetString(array);
        }
    }
}

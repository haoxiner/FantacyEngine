using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;

namespace Editor
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        static bool initialized = false;
        public MainWindow()
        {
            InitializeComponent();
        }

        private void MenuItem_Click(object sender, RoutedEventArgs e)
        {
            if (!initialized) {
                initialized = true;
                
                HwndSource source = (HwndSource)HwndSource.FromVisual(RenderRegion);
                IntPtr hWnd = source.Handle;
                int width = Convert.ToInt32(RenderRegion.ActualWidth);
                int height = Convert.ToInt32(RenderRegion.ActualHeight);
                object args = new object[3] { hWnd, 800, 600};
                Thread newThread = new Thread(DoWork);
                newThread.Start(args);
            }
        }

        private static void DoWork(Object data)
        {
            object[] args = (object[])data;
            IntPtr hWnd = (IntPtr)args[0];
            int width = (int)args[1];
            int height = (int)args[2];
            //MessageBox.Show(width + ", " + height);
            Bridge.Bridge.Initialize(100);
            Bridge.Bridge.SetRenderWindow(hWnd, 0, 0, width, height);
            //Bridge.Bridge.Render();
            //MessageBox.Show(Convert.ToInt32(RenderRegion.ActualWidth).ToString() + ", " + Convert.ToInt32(RenderRegion.ac).ToString());
        }
    }
}

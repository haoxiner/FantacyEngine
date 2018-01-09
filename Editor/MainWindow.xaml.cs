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
                var p = RenderRegion.TranslatePoint(new Point(0.0, 0.0), this);
                //MessageBox.Show(p.X.ToString() + ", " + p.Y.ToString());
                Bridge.Bridge.Initialize(100);
                Bridge.Bridge.SetRenderWindow(hWnd, Convert.ToInt32(p.X), Convert.ToInt32(p.Y),Convert.ToInt32(RenderRegion.ActualWidth), Convert.ToInt32(RenderRegion.ActualHeight));
                Bridge.Bridge.Render();
                //MessageBox.Show(Convert.ToInt32(RenderRegion.ActualWidth).ToString() + ", " + Convert.ToInt32(RenderRegion.ac).ToString());
            }
        }
    }
}

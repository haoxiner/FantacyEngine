using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Editor.Bridge
{
    class Bridge
    {
        [DllImport(@"../../../Debug/Core.dll", EntryPoint = "Initialize", SetLastError = true, CharSet = CharSet.Ansi, ExactSpelling = false, CallingConvention = CallingConvention.StdCall)]
        extern public static int Initialize(int a);
        [DllImport(@"../../../Debug/Core.dll", EntryPoint = "Shutdown", SetLastError = true, CharSet = CharSet.Ansi, ExactSpelling = false, CallingConvention = CallingConvention.StdCall)]
        extern public static void Shutdown();
        [DllImport(@"../../../Debug/Core.dll", EntryPoint = "SetRenderWindow", SetLastError = true, CharSet = CharSet.Ansi, ExactSpelling = false, CallingConvention = CallingConvention.StdCall)]
        extern public static void SetRenderWindow(IntPtr hWnd, int x, int y, int width, int height);
        [DllImport(@"../../../Debug/Core.dll", EntryPoint = "Render", SetLastError = true, CharSet = CharSet.Ansi, ExactSpelling = false, CallingConvention = CallingConvention.StdCall)]
        extern public static void Render();
    }
}

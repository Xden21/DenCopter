using DencopterMonitoring.Application.Views;
using LiveCharts.Geared;
using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
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

namespace DencopterMonitoring.Presentation.Views
{
    /// <summary>
    /// Interaction logic for MotorMonitoringView.xaml
    /// </summary>
    [Export(typeof(IMotorMonitoringView))]
    public partial class MotorMonitoringView : UserControl, IMotorMonitoringView
    {
        public MotorMonitoringView()
        {
            InitializeComponent();
        }

        private void ListBox_PreviewMouseDown(object sender, MouseButtonEventArgs e)
        {
            var item = ItemsControl.ContainerFromElement((ListBox)sender, (DependencyObject)e.OriginalSource) as ListBoxItem;
            if (item == null) return;

            var series = (GLineSeries)item.Content;
            series.Visibility = series.Visibility == Visibility.Visible
                ? Visibility.Hidden
                : Visibility.Visible;
        }
    }
}

using DencopterMonitoring.Application.Views;
using System.Windows.Controls;

namespace DencopterMonitoring.Presentation.Views
{
    /// <summary>
    /// Interaction logic for StatusBarView.xaml
    /// </summary>
    public partial class StatusBarView : Page, IStatusBarView
    {
        public StatusBarView()
        {
            InitializeComponent();
        }
    }
}

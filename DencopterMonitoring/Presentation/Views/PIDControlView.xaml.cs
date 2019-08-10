using DencopterMonitoring.Application.Views;
using System.ComponentModel.Composition;
using System.Windows.Controls;

namespace DencopterMonitoring.Presentation.Views
{
    /// <summary>
    /// Interaction logic for PIDControlView.xaml
    /// </summary>
    [Export(typeof(IPIDControlView))]
    public partial class PIDControlView : UserControl, IPIDControlView
    {
        public PIDControlView()
        {
            InitializeComponent();
        }
    }
}

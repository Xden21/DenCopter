using DencopterMonitoring.Application.Views;
using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Waf.Applications;

namespace DencopterMonitoring.Application.ViewModels
{
    [Export]
    public class PIDControlViewModel : ViewModel<IPIDControlView>
    {
        [ImportingConstructor]
        public PIDControlViewModel(IPIDControlView view) : base(view)
        {
        }
    }
}

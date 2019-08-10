using DencopterMonitoring.Application.Views;
using DencopterMonitoring.Domain;
using LiveCharts.Geared;
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
    public class AngleMonitoringViewModel : ViewModel<IAngleMonitoringView>
    {
        [ImportingConstructor]
        public AngleMonitoringViewModel(IAngleMonitoringView view) : base(view)
        {
            Formatter = value => value.ToString("0.###");

            rollMeasPoints = new GearedValues<LogPoint>().WithQuality(Quality.Medium);
            rollRefPoints = new GearedValues<LogPoint>().WithQuality(Quality.Medium);

            pitchMeasPoints = new GearedValues<LogPoint>().WithQuality(Quality.Medium);
            pitchRefPoints = new GearedValues<LogPoint>().WithQuality(Quality.Medium);

            yawMeasPoints = new GearedValues<LogPoint>().WithQuality(Quality.Medium);
            yawRefPoints = new GearedValues<LogPoint>().WithQuality(Quality.Medium);
        }

        #region Fields & Properties

        private GearedValues<LogPoint> rollMeasPoints;

        public GearedValues<LogPoint> RollMeasPoints
        {
            get { return rollMeasPoints; }
            set { SetProperty(ref rollMeasPoints, value); }
        }

        private GearedValues<LogPoint> rollRefPoints;

        public GearedValues<LogPoint> RollRefPoints
        {
            get { return rollRefPoints; }
            set { SetProperty(ref rollRefPoints, value); }
        }

        private GearedValues<LogPoint> pitchMeasPoints;

        public GearedValues<LogPoint> PitchMeasPoints
        {
            get { return pitchMeasPoints; }
            set { SetProperty(ref pitchMeasPoints, value); }
        }

        private GearedValues<LogPoint> pitchRefPoints;

        public GearedValues<LogPoint> PitchRefPoints
        {
            get { return pitchRefPoints; }
            set { SetProperty(ref pitchRefPoints, value); }
        }

        private GearedValues<LogPoint> yawMeasPoints;

        public GearedValues<LogPoint> YawMeasPoints
        {
            get { return yawMeasPoints; }
            set { SetProperty(ref yawMeasPoints, value); }
        }

        private GearedValues<LogPoint> yawRefPoints;

        public GearedValues<LogPoint> YawRefPoints
        {
            get { return yawRefPoints; }
            set { SetProperty(ref yawRefPoints, value); }
        }

        public Func<double, string> Formatter { get; set; }


        #endregion
    }
}

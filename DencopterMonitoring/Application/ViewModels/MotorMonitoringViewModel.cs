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
    public class MotorMonitoringViewModel : ViewModel<IMotorMonitoringView>
    {
        [ImportingConstructor]
        public MotorMonitoringViewModel(IMotorMonitoringView view) : base(view)
        {
            Formatter = value => value.ToString("0.###");

            motorFLPoints = new GearedValues<LogPoint>().WithQuality(Quality.Medium);
            motorFRPoints = new GearedValues<LogPoint>().WithQuality(Quality.Medium);
            motorBLPoints = new GearedValues<LogPoint>().WithQuality(Quality.Medium);
            motorBRPoints = new GearedValues<LogPoint>().WithQuality(Quality.Medium);
        }

        public Func<double, string> Formatter { get; set; }

        private GearedValues<LogPoint> motorFLPoints;

        public GearedValues<LogPoint> MotorFLPoints
        {
            get { return motorFLPoints; }
            set { SetProperty(ref motorFLPoints, value); }
        }

        private GearedValues<LogPoint> motorFRPoints;

        public GearedValues<LogPoint> MotorFRPoints
        {
            get { return motorFRPoints; }
            set { SetProperty(ref motorFRPoints, value); }
        }

        private GearedValues<LogPoint> motorBLPoints;

        public GearedValues<LogPoint> MotorBLPoints
        {
            get { return motorBLPoints; }
            set { SetProperty(ref motorBLPoints, value); }
        }

        private GearedValues<LogPoint> motorBRPoints;

        public GearedValues<LogPoint> MotorBRPoints
        {
            get { return motorBRPoints; }
            set { SetProperty(ref motorBRPoints, value); }
        }

    }
}

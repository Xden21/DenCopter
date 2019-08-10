using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Waf.Foundation;

namespace DencopterMonitoring.Domain
{
    [Export]
    public class DataSet : Model
    {
        public DataSet()
        { }

        private bool pidIncl;

        public bool PIDIncl
        {
            get { return pidIncl; }
            set { SetProperty(ref pidIncl, value); }
        }

        private PIDData pidData;

        public PIDData PIDData
        {
            get { return pidData; }
            set { SetProperty(ref pidData, value); }
        }


        private bool validInfo;

        public bool ValidInfo
        {
            get { return validInfo; }
            set { SetProperty(ref validInfo, value); }
        }
        
        private int flightMode;

        public int FlightMode
        {
            get { return flightMode; }
            set { SetProperty(ref flightMode, value); }
        }

        private float timeStamp;

        public float TimeStamp
        {
            get { return timeStamp; }
            set { SetProperty(ref timeStamp, value); }
        }

        private int armed;

        public int Armed
        {
            get { return armed; }
            set { SetProperty(ref armed, value); }
        }


        private EulerAngle angleMeasured;

        public EulerAngle AngleMeasured
        {
            get { return angleMeasured; }
            set { SetProperty(ref angleMeasured, value); }
        }

        private EulerAngle angleReference;

        public EulerAngle AngleReference
        {
            get { return angleReference; }
            set { SetProperty(ref angleReference, value); }
        }            

        private MotorSpeeds motorSpeeds;

        public MotorSpeeds MotorSpeeds
        {
            get { return motorSpeeds; }
            set { SetProperty(ref motorSpeeds, value); }
        }              
        
        public override string ToString()
        {
            return FlightMode.ToString() + ";" + TimeStamp.ToString() + ";" + Armed.ToString() + ";" + AngleMeasured.ToString() + AngleReference.ToString() + MotorSpeeds.ToString() + "\n";
        }
    }
}

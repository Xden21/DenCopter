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

        private bool validInfo;

        public bool ValidInfo
        {
            get { return validInfo; }
            set { SetProperty(ref validInfo, value); }
        }

        private bool newPIDData;

        public bool NewPIDData
        {
            get { return newPIDData; }
            set { SetProperty(ref newPIDData, value); }
        }

        private PIDData pIDData;

        public PIDData PIDData
        {
            get { return pIDData; }
            set { SetProperty(ref pIDData, value); }
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

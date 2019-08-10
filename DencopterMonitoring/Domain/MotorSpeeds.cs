using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Waf.Foundation;

namespace DencopterMonitoring.Domain
{
    public class MotorSpeeds : Model
    {
        public MotorSpeeds()
        {

        }

        private float motorFL;

        public float MotorFL
        {
            get { return motorFL; }
            set { SetProperty(ref motorFL, value); }
        }

        private float motorFR;

        public float MotorFR
        {
            get { return motorFR; }
            set { SetProperty(ref motorFR, value); }
        }

        private float motorBL;

        public float MotorBL
        {
            get { return motorBL; }
            set { SetProperty(ref motorBL, value); }
        }

        private float motorBR;

        public float MotorBR
        {
            get { return motorBR; }
            set { SetProperty(ref motorBR, value); }
        }

        public override string ToString()
        {
            return MotorFL.ToString() + ";" + MotorFR.ToString() + ";" + MotorBL.ToString() + ";" + MotorBR.ToString() + ";";
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Waf.Foundation;

namespace DencopterMonitoring.Domain
{
    public class PIDData: Model
    {
        private float yaw_KP;

        public float Yaw_KP
        {
            get { return yaw_KP; }
            set { SetProperty(ref yaw_KP, value); }
        }

        private float yaw_KI;

        public float Yaw_KI
        {
            get { return yaw_KI; }
            set { SetProperty(ref yaw_KI, value); }
        }

        private float yaw_KD;

        public float Yaw_KD
        {
            get { return yaw_KD; }
            set { SetProperty(ref yaw_KD, value); }
        }

        private float pitch_KP;

        public float Pitch_KP
        {
            get { return pitch_KP; }
            set { SetProperty(ref pitch_KP, value); }
        }

        private float pitch_KI;

        public float Pitch_KI
        {
            get { return pitch_KI; }
            set { SetProperty(ref pitch_KI, value); }
        }

        private float pitch_KD;

        public float Pitch_KD
        {
            get { return pitch_KD; }
            set { SetProperty(ref pitch_KD, value); }
        }

        private float roll_KP;

        public float Roll_KP
        {
            get { return roll_KP; }
            set { SetProperty(ref roll_KP, value); }
        }

        private float roll_KI;

        public float Roll_KI
        {
            get { return roll_KI; }
            set { SetProperty(ref roll_KI, value); }
        }

        private float roll_KD;

        public float Roll_KD
        {
            get { return roll_KD; }
            set { SetProperty(ref roll_KD, value); }
        }


        public override string ToString()
        {
            return Yaw_KP + "|" + Yaw_KI + "|" + Yaw_KD + "|" + Pitch_KP + "|" + Pitch_KI + "|" + Pitch_KD + "|" + Roll_KP + "|" + Roll_KI + "|" + Roll_KD;
        }
    }
}

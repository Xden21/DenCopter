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
    public class EulerAngle : Model
    {
        public EulerAngle()
        {
        }

        private float yaw;

        public float Yaw
        {
            get { return yaw; }
            set { SetProperty(ref yaw, value); }
        }

        private float pitch;

        public float Pitch
        {
            get { return pitch; }
            set { SetProperty(ref pitch, value); }
        }

        private float roll;

        public float Roll
        {
            get { return roll; }
            set { SetProperty(ref roll, value); }
        }

        public override string ToString()
        {
            return Yaw.ToString() + ";" + Pitch.ToString() + ";" + Roll.ToString() + ";";
        }
    }
}

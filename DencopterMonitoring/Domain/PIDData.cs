using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Waf.Foundation;

namespace DencopterMonitoring.Domain
{
    public class PIDData : Model
    {
        public event EventHandler DataChanged;

        private PID_DataSet yaw_KP;

        public PID_DataSet Yaw_KP
        {
            get { return yaw_KP; }
            set
            {
                if (yaw_KP != null)
                    yaw_KP.PropertyChanged -= Changed;
                SetProperty(ref yaw_KP, value);
                if (yaw_KP != null)
                    yaw_KP.PropertyChanged += Changed;
            }
        }

        private PID_DataSet yaw_KI;

        public PID_DataSet Yaw_KI
        {
            get { return yaw_KI; }
            set
            {
                if (yaw_KI != null)
                    yaw_KI.PropertyChanged -= Changed;
                SetProperty(ref yaw_KI, value);
                if (yaw_KI != null)
                    yaw_KI.PropertyChanged += Changed;
            }
        }

        private PID_DataSet yaw_KD;

        public PID_DataSet Yaw_KD
        {
            get { return yaw_KD; }
            set
            {
                if (yaw_KD != null)
                    yaw_KD.PropertyChanged -= Changed;
                SetProperty(ref yaw_KD, value);
                if (yaw_KD != null)
                    yaw_KD.PropertyChanged += Changed;
            }
        }

        private PID_DataSet pitch_KP;

        public PID_DataSet Pitch_KP
        {
            get { return pitch_KP; }
            set
            {
                if (pitch_KP != null)
                    pitch_KP.PropertyChanged -= Changed;
                SetProperty(ref pitch_KP, value);
                if (pitch_KP != null)
                    pitch_KP.PropertyChanged += Changed;
            }
        }

        private PID_DataSet pitch_KI;

        public PID_DataSet Pitch_KI
        {
            get { return pitch_KI; }
            set
            {
                if (pitch_KI != null)
                    pitch_KI.PropertyChanged -= Changed;
                SetProperty(ref pitch_KI, value);
                if (pitch_KI != null)
                    pitch_KI.PropertyChanged += Changed;
            }
        }

        private PID_DataSet pitch_KD;

        public PID_DataSet Pitch_KD
        {
            get { return pitch_KD; }
            set
            {
                if (pitch_KD != null)
                    pitch_KD.PropertyChanged -= Changed;
                SetProperty(ref pitch_KD, value);
                if (pitch_KD != null)
                    pitch_KD.PropertyChanged += Changed;
            }
        }

        private PID_DataSet roll_KP;

        public PID_DataSet Roll_KP
        {
            get { return roll_KP; }
            set
            {
                if (roll_KP != null)
                    roll_KP.PropertyChanged -= Changed;
                SetProperty(ref roll_KP, value);
                if (roll_KP != null)
                    roll_KP.PropertyChanged += Changed;
            }
        }

        private PID_DataSet roll_KI;

        public PID_DataSet Roll_KI
        {
            get { return roll_KI; }
            set
            {
                if (roll_KI != null)
                    roll_KI.PropertyChanged -= Changed;
                SetProperty(ref roll_KI, value);
                if (roll_KI != null)
                    roll_KI.PropertyChanged += Changed;
            }
        }

        private PID_DataSet roll_KD;

        public PID_DataSet Roll_KD
        {
            get { return roll_KD; }
            set
            {
                if (roll_KD != null)
                    roll_KD.PropertyChanged -= Changed;
                SetProperty(ref roll_KD, value);
                if (roll_KD != null)
                    roll_KD.PropertyChanged += Changed;
            }
        }

        private int pidMode;

        public int PIDMode
        {
            get { return pidMode; }
            set { SetProperty(ref pidMode, value); }
        }

        public PIDData()
        {
            Yaw_KP = new PID_DataSet(0);
            Yaw_KI = new PID_DataSet(0);
            Yaw_KD = new PID_DataSet(0);

            Pitch_KP = new PID_DataSet(0);
            Pitch_KI = new PID_DataSet(0);
            Pitch_KD = new PID_DataSet(0);

            Roll_KP = new PID_DataSet(0);
            Roll_KI = new PID_DataSet(0);
            Roll_KD = new PID_DataSet(0);
        }

        public override string ToString()
        {
            return Yaw_KP.Value + "|" + Yaw_KI.Value + "|" + Yaw_KD.Value + "|" + Pitch_KP.Value + "|" + Pitch_KI.Value + "|" + Pitch_KD.Value + "|" + Roll_KP.Value + "|" + Roll_KI.Value + "|" + Roll_KD.Value;
        }

        public PIDData Copy()
        {
            return new PIDData()
            {
                Roll_KP = Roll_KP.Copy(),
                Roll_KI = Roll_KI.Copy(),
                Roll_KD = Roll_KD.Copy(),
                Pitch_KP = Pitch_KP.Copy(),
                Pitch_KI = Pitch_KI.Copy(),
                Pitch_KD = Pitch_KD.Copy(),
                Yaw_KP = Yaw_KP.Copy(),
                Yaw_KI = Yaw_KI.Copy(),
                Yaw_KD = Yaw_KD.Copy(),
                PIDMode = PIDMode
            };
        }

        public void SetAll()
        {
            Yaw_KP.Set();
            Yaw_KI.Set();
            Yaw_KD.Set();

            Pitch_KP.Set();
            Pitch_KI.Set();
            Pitch_KD.Set();

            Roll_KP.Set();
            Roll_KI.Set();
            Roll_KD.Set();
        }

        public bool IsChanged()
        {
            return Yaw_KP.Changed || Yaw_KI.Changed || Yaw_KD.Changed
                || Pitch_KP.Changed || Pitch_KI.Changed || Pitch_KD.Changed
                || Roll_KP.Changed || Roll_KI.Changed || Roll_KD.Changed;
        }

        private void Changed(object sender, PropertyChangedEventArgs args)
        {
            if (args.PropertyName == "Value")
                DataChanged?.Invoke(this, EventArgs.Empty);
        }
    }

    public class PID_DataSet : Model
    {
        public PID_DataSet(float value)
        {
            this._value = value;
            Changed = false;
            oldValue = value;
        }

        private float _value;

        public float Value
        {
            get { return _value; }
            set
            {
                if (value != oldValue)
                    Changed = true;
                else
                    Changed = false;
                SetProperty(ref _value, value);
            }
        }

        private float oldValue;

        private bool changed;

        public bool Changed
        {
            get { return changed; }
            set { SetProperty(ref changed, value); }
        }

        public void Set()
        {
            Changed = false;
            oldValue = Value;
        }

        public PID_DataSet Copy()
        {
            return new PID_DataSet(Value);
        }
    }
}

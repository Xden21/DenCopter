using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DencopterMonitoring.Domain
{
    public class LogPoint
    {
        public LogPoint(double timeVal, double value)
        {
            TimeVal = timeVal;
            Value = value;
        }

        public double TimeVal { get; set; }

        public double Value { get; set; }
    }
}

using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DencopterMonitoring.Application.Services
{
    [Export(typeof(IGeneralService))]
    public class GeneralService: IGeneralService
    {
    }
}

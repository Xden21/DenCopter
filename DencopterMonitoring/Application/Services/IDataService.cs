using DencopterMonitoring.Domain;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DencopterMonitoring.Application.Services
{
    public interface IDataService
    {
        event EventHandler<DataUpdateEventArgs> DataUpdateEvent;

        List<DataSet> UnprocessedDataSets { get; set; }

        List<DataSet> AllDataSets { get; set; }

        object DataLock { get; }

        void SaveLoggingToDisk();

        void TriggerDataUpdateEvent(List<DataSet> dataSets);
    }
}

using System.Waf.Applications;

namespace DencopterMonitoring.Application.Views
{
    public interface IShellView: IView
    {
        void Show();

        void Close();
    }
}

﻿using DencopterMonitoring.Application.Views;
using System.ComponentModel.Composition;
using System.Windows.Controls;

namespace DencopterMonitoring.Presentation.Views
{
    /// <summary>
    /// Interaction logic for StatusBarView.xaml
    /// </summary>
    [Export(typeof(IStatusBarView))]
    public partial class StatusBarView : UserControl, IStatusBarView
    {
        public StatusBarView()
        {
            InitializeComponent();
        }
    }
}

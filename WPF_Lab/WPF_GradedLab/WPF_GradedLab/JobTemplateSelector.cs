using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;

namespace WPF_GradedLab
{
    public class JobTemplateSelector : DataTemplateSelector
    {
        public DataTemplate ComboBoxTemplate { get; set; }
        public DataTemplate TextBlockTemplate { get; set; }

        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            var employee = item as Employee;
            if (employee != null && employee.CompanyRole == Role.CEO)
            {
                return TextBlockTemplate;
            }
            return ComboBoxTemplate;
        }
    }
}

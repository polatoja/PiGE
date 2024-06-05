using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WPF_GradedLab
{
    public class EmployeeViewModel : INotifyPropertyChanged
    {
        private Currency _salaryCurrency;
        private Role _companyRole;

        public Currency SalaryCurrency
        {
            get { return _salaryCurrency; }
            set
            {
                if (_salaryCurrency != value)
                {
                    _salaryCurrency = value;
                    OnPropertyChanged(nameof(SalaryCurrency));
                }
            }
        }

        public Role CompanyRole
        {
            get { return _companyRole; }
            set
            {
                if (_companyRole != value)
                {
                    _companyRole = value;
                    OnPropertyChanged(nameof(CompanyRole));
                }
            }
        }

        public EmployeeViewModel()
        {
            SalaryCurrency = Currency.PLN;
            CompanyRole = Role.Worker;
        }
        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }

}

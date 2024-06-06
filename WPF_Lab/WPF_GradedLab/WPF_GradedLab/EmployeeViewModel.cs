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
        private DateTime _birthDate;
        private decimal _salary;

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
        public DateTime BirthDate
        {
            get { return _birthDate; }
            set
            {
                if (_birthDate != value)
                {
                    _birthDate = value;
                    OnPropertyChanged(nameof(BirthDate));
                }
            }
        }
        public decimal Salary
        {
            get { return _salary; }
            set
            {
                if (_salary != value)
                {
                    _salary = value;
                    OnPropertyChanged(nameof(Salary));
                }
            }
        }
        public EmployeeViewModel()
        {
            SalaryCurrency = Currency.PLN;
            CompanyRole = Role.Worker;
            BirthDate = DateTime.Now.AddYears(-30);
            Salary = 5000;
        }
        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }

}

using System.Globalization;
using System.Windows.Controls;

namespace WPF_GradedLab
{
    public class SalaryValidationRule : ValidationRule
    {
        public double MinSalary { get; set; }
        public override ValidationResult Validate(object value, CultureInfo cultureInfo)
        {
            if (value != null && double.TryParse(value.ToString(), out double salary))
            {
                if (salary < MinSalary)
                {
                    return new ValidationResult(false, "Minimum salary is 5000. That is too low.");
                }
            }

            return ValidationResult.ValidResult;
        }
    }
}

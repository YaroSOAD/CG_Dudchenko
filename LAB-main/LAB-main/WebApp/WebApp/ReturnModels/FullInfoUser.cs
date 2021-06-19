using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace WebApp.ReturnModels
{
    public class FullInfoUser
    {
        public long Id { get; set; }
        public string Name { get; set; }
        public long ItemId { get; set; }
        public string ItemName { get; set; }
        public string Category { get; set; }
        public string Importance { get; set; }
        public DateTime? StartTime { get; set; }
        public DateTime? EndTime { get; set; }
        public bool IsComplete { get; set; }
    }
}

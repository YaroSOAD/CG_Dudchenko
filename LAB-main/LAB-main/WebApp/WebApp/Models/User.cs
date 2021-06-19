using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Threading.Tasks;

namespace WebApp.Models
{
    public class User
    {
        public long Id { get; set; }
        public string Name { get; set; }

       public List<TodoItem> TodoItems = new List<TodoItem>();
    }
}

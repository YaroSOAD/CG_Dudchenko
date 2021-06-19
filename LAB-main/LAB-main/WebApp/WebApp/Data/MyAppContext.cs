using Microsoft.EntityFrameworkCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using WebApp.Models;

namespace WebApp.Data
{
    public class MyAppContext: DbContext
    {
        public DbSet<TodoItem> TodoItems { get; set; }
        public DbSet<User> Users { get; set; }
        public MyAppContext(DbContextOptions<MyAppContext> options)
            : base(options)
        {
            Database.EnsureCreated();
        }
    }
}

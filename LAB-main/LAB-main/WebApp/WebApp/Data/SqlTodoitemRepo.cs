using Microsoft.EntityFrameworkCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using WebApp.Models;

namespace WebApp.Data
{
    public class SqlTodoitemRepo : ITodoItemRepo
    {
        private readonly MyAppContext _context;

        private readonly DateTime _startTime = new DateTime(2000, 1, 1, 0, 0, 0);
        private readonly DateTime _endTime = new DateTime(3000, 1, 1, 0, 0, 0);

        public SqlTodoitemRepo(MyAppContext context)
        {
            _context = context;
        }
        public void CreateItem(TodoItem item)
        {
            if (item == null)
            {
                throw new ArgumentNullException(nameof(item));
            }

            _context.TodoItems.Add(item);
        }

        public void DeleteItem(TodoItem item)
        {
            if (item == null)
            {
                throw new ArgumentNullException(nameof(item));
            }
            _context.TodoItems.Remove(item);
        }

        public async Task<IEnumerable<TodoItem>> GetAllItems()
        {
            return await _context.TodoItems.ToListAsync();
        }

        public async Task<TodoItem> GetItemById(long id)
        {
            return await _context.TodoItems.FindAsync(id);
        }

        public void UpdateItem(TodoItem item)
        {
            _context.Entry(item).State = EntityState.Modified;
        }

        public async Task<bool> SaveChanges()
        {
            return (await _context.SaveChangesAsync() >= 0);
        }

        public IEnumerable<TodoItem> UserItemsByCategory(long id, string category)
        {
            return from item in _context.TodoItems where (item.UserId == id) && (item.Category == category) orderby item.Category ascending select item;
        }

        public IEnumerable<TodoItem> UserItemsByImportance(long id, string importance)
        {
            return from item in _context.TodoItems where (item.UserId == id) & (item.Importance == importance) select item;
        }

        public IEnumerable<TodoItem> UserItemsByTime(long id, DateTime? StartTime = null, DateTime? EndTime = null)
        {
            if (StartTime == null && EndTime == null)
            {
                return from item in _context.TodoItems where (item.UserId == id) & (item.StartTime == null) & (item.EndTime == null) select item;
            }
            else if (StartTime == null)
            {
                return from item in _context.TodoItems where (item.UserId == id) & (item.EndTime <= EndTime) select item;
            }
            else if (EndTime == null)
            {
                return from item in _context.TodoItems where (item.UserId == id) & (item.StartTime <= StartTime) select item;
            }

            else
            {
                return from item in _context.TodoItems where (item.UserId == id) & (item.StartTime >= StartTime) & (item.EndTime <= EndTime) select item;
            }
        }

        public IEnumerable<TodoItem> UserItemsIsComplete(long id, bool IsComplete) 
        {
            return from items in _context.TodoItems where (items.UserId == id) && (items.IsComplete == IsComplete) select items;
        }
    }
}

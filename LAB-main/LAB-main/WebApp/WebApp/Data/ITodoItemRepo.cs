using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using WebApp.Models;

namespace WebApp.Data
{
    public interface ITodoItemRepo
    { 
        Task<IEnumerable<TodoItem>> GetAllItems();
        Task<TodoItem> GetItemById(long id);
        void CreateItem(TodoItem item);
        void UpdateItem(TodoItem item);
        void DeleteItem(TodoItem item);
        Task<bool> SaveChanges();
        IEnumerable<TodoItem> UserItemsByCategory(long id, string category);
        IEnumerable<TodoItem> UserItemsByImportance(long id, string importance);
        IEnumerable<TodoItem> UserItemsByTime(long id, DateTime? StartTime = null, DateTime? EndTime = null);
        IEnumerable<TodoItem> UserItemsIsComplete(long id, bool IsComplete);
    }
}

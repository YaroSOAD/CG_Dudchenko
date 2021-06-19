using Microsoft.EntityFrameworkCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using WebApp.Models;
using WebApp.ReturnModels;

namespace WebApp.Data
{
    public class SqlUserRepo : IUserRepo
    {
        private readonly MyAppContext _context;

        public SqlUserRepo(MyAppContext context)
        {
            _context = context;
        }
        public void CreateUser(User user)
        {
            if (user == null)
            {
                throw new ArgumentNullException(nameof(user));
            }

            _context.Users.Add(user);
        }


        public void DeleteUser(User user)
        {
            if (user == null)
            {
                throw new ArgumentNullException(nameof(user));
            }
            _context.Users.Remove(user);
        }

        public async Task<IEnumerable<User>> GetAllUsers()
        {
            return  await _context.Users.ToListAsync();
        }

        public async Task<User> GetUserById(long id)
        {
            return await _context.Users.FindAsync(id);
        }

        public void UpdateUser(User user)
        {

            _context.Entry(user).State = EntityState.Modified;
        }

        public async Task<bool> SaveChanges()
        {

            return (await _context.SaveChangesAsync() >= 0);
        }

        public IEnumerable<FullInfoUser> UsersInfo(long id)
        {
            var users = from user in _context.Users
                        join todoItem in _context.TodoItems on user.Id equals todoItem.UserId orderby user.Name
                        where user.Id == id
                        select new FullInfoUser
                        {

                            Id = user.Id,
                            Name = user.Name,
                            ItemId = todoItem.Id,
                            ItemName = todoItem.Name,
                            Category = todoItem.Category,
                            Importance = todoItem.Importance,
                            IsComplete = todoItem.IsComplete
                        };
            return users;
        }

        public IEnumerable<FullInfoUser> UsersInfo()
        {
            var users = from user in _context.Users
                        join todoItem in _context.TodoItems on user.Id equals todoItem.UserId orderby user.Name
                        select new FullInfoUser
                        {

                            Id = user.Id,
                            Name = user.Name,
                            ItemId = todoItem.Id,
                            ItemName = todoItem.Name,
                            Category = todoItem.Category,
                            Importance = todoItem.Importance,
                            IsComplete = todoItem.IsComplete
                        };
            return users;
        }
    }
}

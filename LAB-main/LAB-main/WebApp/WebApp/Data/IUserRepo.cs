using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using WebApp.Models;
using WebApp.ReturnModels;

namespace WebApp.Data
{
    public interface IUserRepo
    { 
        Task<IEnumerable<User>> GetAllUsers();
        Task<User> GetUserById(long id);
        void CreateUser(User user);
        void UpdateUser(User user);
        void DeleteUser(User user);
        Task<bool> SaveChanges();
        IEnumerable<FullInfoUser> UsersInfo(long id);
        IEnumerable<FullInfoUser> UsersInfo();
    }
}

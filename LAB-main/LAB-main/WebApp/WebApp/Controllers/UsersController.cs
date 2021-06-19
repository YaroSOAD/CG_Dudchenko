using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using WebApp.Data;
using WebApp.Models;
using WebApp.ReturnModels;

namespace WebApp.Controllers
{
    [Route("api/[controller]")]

                                                                                //public ActionResult<IEnumerable<User>> GetUsers1(User user1)
                                                                                //{
                                                                                //    var users = from user in _context.Users where user.Name == user1.Name select user;
                                                                                //    return users.ToList();  //await _context.Users.ToListAsync();
                                                                                //}
    [ApiController]
    public class UsersController : ControllerBase
    {
        private readonly IUserRepo _repository;

        public UsersController(IUserRepo repository)
        {
            _repository = repository;
        }


        // GET: api/Users
        [HttpGet]
        
        public IEnumerable<User> GetUsers()
        { 
            return _repository.GetAllUsers().Result;
        }

        // GET: api/Users/5
        [HttpGet("{id}")]
        public ActionResult<User> GetUser(long id)
        {
            var user = _repository.GetUserById(id).Result;

            if (user == null)
            {
                return NotFound();
            }

            return user;
        }

        // PUT: api/Users/5
        // To protect from overposting attacks, enable the specific properties you want to bind to, for
        // more details, see https://go.microsoft.com/fwlink/?linkid=2123754.
        [HttpPut("{id}")]
        public IActionResult PutUser(long id, User user)
        {
            var userFromRepo = _repository.GetUserById(id).Result;
            if (userFromRepo == null)
            {
                return NotFound();
            }

            userFromRepo.Name = user.Name;

            _repository.UpdateUser(userFromRepo );
            
            _repository.SaveChanges();

            return NoContent();
        }

        // POST: api/Users
        // To protect from overposting attacks, enable the specific properties you want to bind to, for
        // more details, see https://go.microsoft.com/fwlink/?linkid=2123754.
        [HttpPost]
        public ActionResult<User> PostUser(User user)
        {
            _repository.CreateUser(user);
            _repository.SaveChanges();

            return CreatedAtAction("GetUser", new { id = user.Id }, user);
        }

        // DELETE: api/Users/5
        [HttpDelete("{id}")]
        public ActionResult<User> DeleteUser(long id)
        {
            var user = _repository.GetUserById(id).Result;
            if (user == null)
            {
                return NotFound();
            }

            _repository.DeleteUser(user);
            _repository.SaveChanges();

            return user;
        }

        //api/[controller]/Users/Info/1
        [HttpGet("Info/{id}")]
        public IEnumerable<FullInfoUser> UsersInfo(long id)
        {
            return _repository.UsersInfo(id);
        }

        //api/[controller]/Users/Info/
        [HttpGet("Info")]
        public IEnumerable<FullInfoUser> UsersInfo()
        {
            return _repository.UsersInfo();
        }

    }
}

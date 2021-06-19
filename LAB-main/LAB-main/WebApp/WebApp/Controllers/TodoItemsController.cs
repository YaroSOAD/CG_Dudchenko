using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using WebApp.Data;
using WebApp.Models;

namespace WebApp.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class TodoItemsController : ControllerBase
    {

        private readonly ITodoItemRepo _repository;

        public TodoItemsController(ITodoItemRepo repository)
        {
            _repository = repository;
        }

        // GET: api/TodoItems
        [HttpGet]
        public IEnumerable<TodoItem> GetTodoItems()
        {
            return _repository.GetAllItems().Result;
        }

        // GET: api/TodoItems/5
        [HttpGet("{id}")]
        public ActionResult<TodoItem> GetTodoItem(long id)
        {
            var todoItem = _repository.GetItemById(id).Result;

            if (todoItem == null)
            {
                return NotFound();
            }

            return todoItem;
        }

        // PUT: api/TodoItems/5
        // To protect from overposting attacks, enable the specific properties you want to bind to, for
        // more details, see https://go.microsoft.com/fwlink/?linkid=2123754.
        [HttpPut("{id}")]
        public  ActionResult PutTodoItem(long id, TodoItem todoItem)
        {
            var ItemFromRepo = _repository.GetItemById(id).Result;
            if(ItemFromRepo == null) 
            {
                return NotFound();
            }

            ItemFromRepo.Name = todoItem.Name;
            ItemFromRepo.IsComplete = todoItem.IsComplete;
            ItemFromRepo.Importance = todoItem.Importance;
            ItemFromRepo.Category = ItemFromRepo.Category;
            ItemFromRepo.UserId = todoItem.UserId;

            _repository.UpdateItem(ItemFromRepo);

            _repository.SaveChanges();

            return NoContent();
        }

        // POST: api/TodoItems
        // To protect from overposting attacks, enable the specific properties you want to bind to, for
        // more details, see https://go.microsoft.com/fwlink/?linkid=2123754.
        [HttpPost]
        public ActionResult PostTodoItem(TodoItem todoItem)
        {
            _repository.CreateItem(todoItem);
            _repository.SaveChanges();

            return CreatedAtAction(nameof(GetTodoItem), new { id = todoItem.Id }, todoItem);
        }

        // DELETE: api/TodoItems/5
        [HttpDelete("{id}")]
        public  ActionResult<TodoItem> DeleteTodoItem(long id)
        {
            var todoItem = _repository.GetItemById(id).Result;
            if (todoItem == null)
            {
                return NotFound();
            }

            _repository.DeleteItem(todoItem);
            _repository.SaveChanges();

            return todoItem;
        }

        [HttpGet("ByCategory/{id}")]
        public IEnumerable<TodoItem> UserItemsByCategory(long id, [FromQuery]string category)
        {
            return _repository.UserItemsByCategory(id, category);
        }

        [HttpGet("ByImportance/{id}")]
        public IEnumerable<TodoItem> UserItemsByImportance(long id, [FromQuery] string importance)
        {
            return _repository.UserItemsByImportance(id, importance);
        }

        [HttpGet("ByTime/{id}")]
        public IEnumerable<TodoItem> UserItemsByTime(long id, [FromQuery] DateTime? StartTime = null, [FromQuery]DateTime? EndTime = null)
        {
            return _repository.UserItemsByTime(id, StartTime, EndTime);
        }

        [HttpGet("IsComplete/{id}")]
        public IEnumerable<TodoItem> UserItemsIsComplete(long id, [FromQuery] bool IsComplete)
        {
            return _repository.UserItemsIsComplete(id, IsComplete);
        }


    }

}

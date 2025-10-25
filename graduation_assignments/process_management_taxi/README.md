# Система управления такси

**Цель**: Реализация системы управления такси с использованием процессов и межпроцессного взаимодействия. CLI приложение создает и управляет driver процессами, которые выполняют задачи с заданным временем.

**Команды CLI**:
- **`create_driver`** - создает новый процесс driver
- **`send_task <pid> <task_timer>`** - назначает задачу driver с указанным PID на заданное количество секунд
- **`get_status <pid>`** - показывает статус driver (Available или Busy с оставшимся временем)
- **`get_drivers`** - показывает статусы всех активных drivers
- **`exit`** - завершает работу системы

**Для запуска**:
- Ввести `make`
- Ввести `./taxi_manager`

**Пример работы**:
```bash
Taxi manager started ('exit' to quit)
Available commands: create_driver, send_task <pid> <timer>, get_status <pid>, get_drivers
create_driver
Created driver with PID: 4407
create_driver
Created driver with PID: 4409
send_task 4409 60
Driver 4409 started task 60
get_status 4409
Driver 4409: Busy 50
get_drivers
Active drivers:
Driver 4407: Available
Driver 4409: Busy 29
exit
Waiting for drivers to finish...

Taxi manager shat down
```

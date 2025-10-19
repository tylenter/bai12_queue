CÁC BƯỚC THỰC HIỆN:
1.Cấu hình GPIO:
  - Dùng chân PA5 làm chân xuất tín hiệu:

<img width="1014" height="290" alt="image" src="https://github.com/user-attachments/assets/e1a23fb0-22c7-4ac4-9d92-6375a9e4810a" />

2. Các luồng hoạt động của task:
     - Định nghĩa kiểu nháy: Một mảng tên patterns được tạo để lưu sẵn 4 kiểu nháy khác nhau (tần số và chu kỳ làm việc).
     - Vòng lặp vô tận:
     - Lấy một kiểu nháy từ mảng ra.
     - Dùng xQueueSend() để đẩy kiểu nháy đó vào ledQueueHandle.
     - Dùng vTaskDelay() để tạm dừng task này trong 2000ms (2 giây).
     - Sau 2 giây, task hoạt động trở lại và lặp lại quy trình với kiểu nháy tiếp theo.
       
<img width="899" height="512" alt="image" src="https://github.com/user-attachments/assets/dbf01228-907d-44ef-bbef-bc67f6a699aa" />


  - Task vLedBlinkTask (Nhận Dữ liệu và Điều khiển LED)
  - Chi tiết:
    + Nhận dữ liệu lần đầu: Dùng xQueueReceive() với tham số portMAX_DELAY để chờ vô thời hạn cho đến khi có dữ liệu đầu tiên trong queue.
    + Tính toán thời gian: Sau khi nhận được, nó sẽ tính toán thời gian BẬT (on_time_ms) và TẮT (off_time_ms) dựa trên tần số và chu kỳ làm việc.
    + Vòng lặp vô tận:
       * Liên tục kiểm tra xem có dữ liệu mới trong queue không bằng cách gọi xQueueReceive() với thời gian chờ là 0. Nếu có, nó sẽ cập nhật lại on_time_ms và off_time_ms.
       * Thực hiện nhấp nháy LED: Bật LED (Bit_SET), tạm dừng on_time_ms, sau đó Tắt LED (Bit_RESET), và tạm dừng off_time_ms.

<img width="905" height="335" alt="image" src="https://github.com/user-attachments/assets/69dfea71-1c34-4447-bfab-44c8377fc064" />

<img width="925" height="450" alt="image" src="https://github.com/user-attachments/assets/85bd58ce-c13b-4d1f-bed3-8a53977ef70a" />

3. Chương trình chính:
   - Cấu hình phần cứng:
   - Tạo queue
   - Tạo các task
   - Khởi động bộ lệnh
  
  <img width="834" height="486" alt="image" src="https://github.com/user-attachments/assets/67fdbd66-d297-48a5-bc68-243b05d68ff3" />

4. Kết quả:
   - Led trên chân PA5 sẽ nháy và thay đổi kiểu nháy mỗi 2s.


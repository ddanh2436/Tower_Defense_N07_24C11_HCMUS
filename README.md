🚩 TOWER DEFENSE_N07_24C11_HCMUS
Đây là một dự án game Tower Defense cổ điển được xây dựng bằng ngôn ngữ C++ và thư viện đồ họa SFML (Simple and Fast Multimedia Library). Người chơi sẽ phải sử dụng tư duy chiến thuật để xây dựng các tháp phòng thủ, ngăn chặn các đợt tấn công của kẻ thù.

✨ TÍNH NĂNG NỔI BẬT
   Lối chơi chiến thuật: Xây dựng và nâng cấp các loại tháp khác nhau để chống lại từng loại kẻ thù.

🗺️ Nhiều bản đồ: Game hỗ trợ nhiều bản đồ với các đường đi của quái vật khác nhau, đòi hỏi chiến thuật riêng cho từng màn.

♞♜ Đa dạng kẻ thù và tháp:

Kẻ thù: Nhiều loại quái vật với các chỉ số máu, tốc độ, và giá trị tiền thưởng khác nhau (Goblin, Sói, Ong, Slime, v.v.).

Tháp: Hiện tại có tháp cung thủ (Archer Tower) và tháp pháo binh (Cannon Tower) với nhiều cấp độ nâng cấp, thay đổi về sức mạnh và hình ảnh.

💾 Hệ thống lưu/tải game: Người chơi có thể lưu lại tiến trình của mình và tiếp tục chơi sau.

🥇 Bảng xếp hạng (Leaderboard): Ghi lại điểm số cao của người chơi, bao gồm tên, điểm, số quái đã diệt và thời gian hoàn thành.

🎵 Quản lý âm thanh: Tùy chỉnh âm lượng nhạc nền, hiệu ứng âm thanh, và có thể bật/tắt chúng trong phần cài đặt.

🎨 Đồ họa và hiệu ứng: Game sử dụng các sprite và animation để tạo ra trải nghiệm sống động.

🖥️ Giao diện người dùng (UI) đầy đủ: Bao gồm menu chính, màn hình chọn bản đồ, màn hình cài đặt, menu tạm dừng trong game và các thông báo khác.

🛠️ CÀI ĐẶT VÀ YÊU CẦU
Yêu cầu hệ thống

🖥 Hệ điều hành: Windows.

Một trình biên dịch C++ hỗ trợ C++17 trở lên (ví dụ: Visual Studio 2019/2022).

📚 Thư viện cần thiết
SFML (>= 2.5.1): Thư viện đồ họa và đa phương tiện chính của dự án.

Bạn cần tải bản phù hợp với trình biên dịch của mình (ví dụ: Visual C++ 15 (2017) - 64-bit).

Hãy chắc chắn rằng bạn đã thiết lập đúng các đường dẫn Include Directories và Library Directories trong project, cũng như liên kết các file .lib cần thiết (sfml-graphics-d.lib, sfml-window-d.lib, sfml-system-d.lib, sfml-audio-d.lib, v.v. cho Debug và không có -d cho Release).

🌳📁 Cấu trúc thư mục
Để dự án chạy đúng, hãy đảm bảo các thư mục được sắp xếp như sau:

Tower/
|-- Tower.vcxproj
|-- MainPrg.cpp
|-- cgame.cpp
|-- cgame.h
|-- cmap.cpp
|-- cmap.h
|-- ... (các file .cpp và .h khác)
|
|-- assets/
|   |-- pixel_font.ttf
|   |-- menu_background.png
|   |-- menu_music.ogg
|   |-- game_music.ogg
|   |-- ... (tất cả các file hình ảnh và âm thanh khác)
|
|-- data/
|   |-- maps_index.txt
|   |-- map1.txt
|   |-- map2.txt
|   |-- map3.txt
|   |-- map4.txt
|   |-- savegame.txt
|   |-- leaderboards.txt
|
|-- include/
|   |-- SFML/
|       |-- (Toàn bộ các file header của SFML)
|
|-- lib/
|   |-- (Toàn bộ các file .lib của SFML)
|
|-- bin/
    |-- (Toàn bộ các file .dll của SFML)
Lưu ý quan trọng: File thực thi (.exe) sau khi biên dịch cần nằm cùng cấp với thư mục assets/ và data/ để game có thể tìm thấy tài nguyên.

🚀 CÁCH SỬ DỤNG

Biên dịch game: Mở dự án Tower.vcxproj trong Visual Studio, chọn cấu hình Debug hoặc Release, và biên dịch dự án. Đảm bảo rằng các thư viện SFML đã được liên kết đúng.

Chạy game: Chạy file Tower.exe đã được tạo ra.

Menu chính: Hiển thị các tuỳ chọn trong game.

New Game: Bắt đầu một màn chơi mới. Bạn sẽ được chuyển đến màn hình chọn bản đồ.

Load Game: Tải lại màn chơi đã được lưu từ file data/savegame.txt.

Leaderboard: Xem bảng xếp hạng điểm cao.

Settings: Tùy chỉnh âm thanh của game.

Exit: Thoát khỏi trò chơi.

⚔️⚔️ Trong game:

♜ Xây tháp: Có 1 thanh hiển thị danh sách trụ ở bên dưới, góc phải màn hình game. Có hai loại trụ để người chơi chọn lựa,

lần lượt là "Archer Tower" và "Cannon Tower". Người chơi đặt trụ bằng cách nhấn chuột vào một trong các loại trụ, di chuyển chuột đến vị trí có thể xây (ô màu xanh lá) và nhấn chuột trái để đặt tháp.

Người chơi có thể xây và bán tháp trước khi trận đấu bắt đầu.

⚒ Nâng cấp/Bán tháp: Nhấn chuột trái vào một tháp đã xây để hiện ra bảng điều khiển. Tại đây bạn có thể chọn nâng cấp hoặc bán tháp.

▶️ Bắt đầu đợt tấn công: Nhấn phím N khi sẵn sàng để bắt đầu đợt tấn công tiếp theo.

⏸ Tạm dừng: Nhấn phím Esc để tạm dừng game. Trong menu tạm dừng, bạn có thể tiếp tục, khởi động lại hoặc thoát ra menu chính (với tùy chọn lưu game).

⏩ Tua nhanh: Nhấn vào nút tua nhanh ở góc trên bên phải để tăng tốc độ game.

🎯 Mục tiêu:

Ngăn chặn kẻ thù đi đến cuối con đường. Mỗi kẻ thù lọt qua sẽ làm bạn mất một mạng. Bạn sẽ thua cuộc khi mất hết mạng. ❤️

Sống sót qua tất cả các đợt tấn công để chiến thắng màn chơi! 💥

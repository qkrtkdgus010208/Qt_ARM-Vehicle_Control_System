#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <QCamera>
#include <QCameraInfo>
#include <QAbstractVideoSurface>
#include <QVideoFrame>
#include <QImage>
// ============================================================================
// [Class] VideoSurface
// 역할: 카메라 하드웨어로부터 실시간 비디오 프레임을 수신하여 콜백 함수로 토스하는 클래스
// ============================================================================
class VideoSurface : public QAbstractVideoSurface {
    Q_OBJECT
public:
	// 생성자: 프레임 수신 시 실행할 콜백 함수(std::function)를 등록받음
    explicit VideoSurface(std::function<void(const QVideoFrame&)> callback, QObject *parent = nullptr)
        : QAbstractVideoSurface(parent), m_callback(callback) {}
		
	// Qt 멀티미디어 시스템에 이 수신기가 처리 가능한 이미지 포맷(RGB32, ARGB32)을 알려줌
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const override {
        Q_UNUSED(handleType);
        return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_RGB32 << QVideoFrame::Format_ARGB32;
    }
	
	// 카메라에 새 프레임이 찍힐 때마다 자동 호출되며, 등록된 콜백 함수로 프레임을 전달
    bool present(const QVideoFrame &frame) override {
        if (frame.isValid()) {
            m_callback(frame);
        }
        return true;
    }

private:
	// 호출할 함수 주소를 저장하는 포인터
    std::function<void(const QVideoFrame&)> m_callback;
};

// ============================================================================
// [Class] CameraThread
// 역할: 백그라운드 스레드에서 카메라를 구동하고 영상을 가공하여 메인 GUI로 전송하는 클래스
// ============================================================================
class CameraThread : public QThread {
    Q_OBJECT
public:
    explicit CameraThread(QObject *parent = nullptr); // 생성자
    ~CameraThread(); // 소멸자 (자원 해제 처리 예정)

signals:
	// 가공이 완료된 이미지를 메인 윈도우(GUI)로 보낼 때 발생하는 Qt 시그널
    void send_image(const QImage &image);

protected:
	// QThread의 메인 실행 루프 (start() 호출 시 백그라운드에서 실행되는 함수)
    void run() override;

private:
	// --- 카메라 제어 관련 내부 단계별 함수 ---
	int init_capture();       // 카메라 검색 및 초기화 설정
    int start_capture();      // 카메라 영상 캡처 시작
    void stop_capture();      // 카메라 영상 캡처 중지
    void close_capture();     // 카메라 장치 연결 닫기 및 자원 해제
    
    // --- 영상 데이터 처리 내부 함수 ---
    void frame_receive(const QVideoFrame& frame);       // [참고] 프레임 임시 수신용 함수 (필요시 사용)
    void process_video_frame(const QVideoFrame &frame); // 수신된 프레임을 변환/반전 가공하는 핵심 로직

    // --- 멤버 변수 ---
    QCamera *m_camera;               // Qt 카메라 장치 제어 객체 포인터
    VideoSurface *m_video_surface;   // 커스텀 비디오 프레임 수신기(도화지) 객체 포인터
    int m_frame_count;               // 누적 수신된 프레임 수를 세는 카운터 변수
    int m_frame_divisor;             // 프레임 스킵/솎아내기 기준값 (분모)
};

#endif // CAMERATHREAD_H
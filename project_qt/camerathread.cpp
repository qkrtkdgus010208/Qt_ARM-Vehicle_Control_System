#include "camerathread.h"
#include <QDebug>

// ============================================================================
// CameraThread 생성자 (멤버 변수 안전 초기화)
// ============================================================================
CameraThread::CameraThread(QObject *parent)
    : QThread(parent)
    , m_camera(nullptr)
    , m_video_surface(nullptr)
    , m_frame_count(0)
    , m_frame_divisor(1) 
{
}

// ============================================================================
// CameraThread 소멸자 (스레드 안전 종료 보장)
// ============================================================================
CameraThread::~CameraThread() {
    // 스레드가 아직 실행 중이라면 run() 함수가 완전히 끝날 때까지 대기하여 안전한 파괴를 보장
    wait();
}

// ============================================================================
// 스레드 메인 실행 함수 (start() 호출 시 백그라운드에서 자동 실행)
// ============================================================================
void CameraThread::run() {
    // 1. 카메라 장치 및 수신기 초기화 (실패 시 스레드 즉시 종료)
    if (init_capture() < 0) return;
    
    // 2. 카메라 영상 스트리밍 시작 (실패 시 스레드 즉시 종료)
    if (start_capture() < 0) return;

    // 3. 비동기 이벤트 루프 구동 (quit() 신호가 올 때까지 여기서 대기하며 이벤트 처리)
    exec();

    // 4. 이벤트 루프가 종료되면 카메라를 멈추고 자원을 해제
    stop_capture();
    close_capture();
}

// ============================================================================
// 카메라 및 비디오 수신기(Surface) 초기화 함수
// ============================================================================
int CameraThread::init_capture() {
    // 이미 카메라 객체가 생성되어 있다면(중복 초기화 방지) 정상 종료(0) 반환
    if (m_camera) return 0;

    // 시스템(PC)에 사용 가능한 카메라 장치 목록을 불러옴
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    if (cameras.isEmpty()) {
        qDebug() << "No camera detected!";
        return -1;
    }

    // 디버그 콘솔에 발견된 카메라 목록 출력
    qDebug() << "=== Detected Camera List ===";
    for (int i = 0; i < cameras.size(); ++i) {
        qDebug() << "Index [" << i << "] :" << cameras[i].description();
    }
    qDebug() << "===============================";

    // 카메라가 2개 이상이면 1번(외장 카메라), 1개뿐이면 0번(기본 내장 카메라)을 타겟으로 설정
    int target_index = (cameras.size() > 1) ? 1 : 0;
    qDebug() << "Current Camera:" << cameras[target_index].description();

    // 선택한 카메라 정보를 바탕으로 실제 제어할 QCamera 객체를 동적 생성
    m_camera = new QCamera(cameras[target_index]);

    // 영상 수신기(VideoSurface)를 생성하면서, 
    // 새 프레임이 들어올 때마다 클래스 내부 함수(process_video_frame)를 실행하도록 람다식으로 묶어줌
    m_video_surface = new VideoSurface([this](const QVideoFrame& frame) {
        this->process_video_frame(frame);
    });

    // 카메라의 뷰파인더(출력 대상)를 우리가 만든 영상 수신기(m_video_surface)로 지정하여 영상을 그리도록 설정
    m_camera->setViewfinder(m_video_surface);
    return 0;
}

// ============================================================================
// 수신된 비디오 프레임 가공 및 메인 UI 전송 함수
// ============================================================================
void CameraThread::process_video_frame(const QVideoFrame &frame) {
    // 유효하지 않은 데이터 프레임이라면 즉시 제외
    if (!frame.isValid()) return;

    QVideoFrame clone_frame(frame);

    // CPU가 비디오 데이터 메모리에 접근할 수 있도록 읽기 전용(ReadOnly) 매핑 시도
    if (clone_frame.map(QAbstractVideoBuffer::ReadOnly)) {
        try {
            // 이미지 생성을 위한 프레임 정보 추출
            int width = clone_frame.width();
            int height = clone_frame.height();
            int bytes_per_line = clone_frame.bytesPerLine();
            uchar *ptr = clone_frame.bits();

            // 메모리 주소가 정상이라면
            if (ptr != nullptr) {
                // 원본 메모리 주소를 참조하여 QImage 생성 (복사 없는 빠른 래핑)
                QImage img(ptr, width, height, bytes_per_line, QImage::Format_RGB32);

                if (!img.isNull()) {
                    m_frame_count++; // 프레임 카운트 증가
                    
                    // 정해진 분모(Divisor) 주기에 해당할 때만 가공 및 전송 (프레임 스킵 기법)
                    if (m_frame_count % m_frame_divisor == 0) {
                        
                        // 1. 범용적인 표준 포맷(RGBA8888)으로 변환
                        QImage standard_img = img.convertToFormat(QImage::Format_RGBA8888);

                        // 2. 상하 반전 (카메라 센서 특성 및 그래픽 좌표계 뒤집힘 보정)
                        QImage flipped_img = standard_img.mirrored(false, true);

                        // 3. 메인 윈도우(GUI)로 완성된 이미지 전송 신호 발생
                        emit send_image(flipped_img);
                    }
                }
            }
        } catch (...) {
            // 프레임 변환 과정 중 예외 발생 시 크래시 방지용 예외 처리
            qDebug() << "Frame Conversion Error";
        }
        
        // 메모리 매핑 해제 (자원 누수 방지를 위해 필수)
        clone_frame.unmap();
    }
}

// ============================================================================
// 카메라 스트리밍 시작 함수
// ============================================================================
int CameraThread::start_capture() {
    if (m_camera) {
        m_camera->start(); // 하드웨어 카메라 가동 시작
        qDebug() << "QCamera Stream on...";
        return 0;
    }
    return -1;
}

// ============================================================================
// 카메라 스트리밍 중지 함수
// ============================================================================
void CameraThread::stop_capture() {
    if (m_camera) {
        m_camera->stop(); // 하드웨어 카메라 가동 중지
        qDebug() << "QCamera Stream off!!";
    }
}

// ============================================================================
// 메모리 자원 최종 해제 및 초기화 함수
// ============================================================================
void CameraThread::close_capture() {
    // 1. 동적 할당된 카메라 객체 메모리 해제 및 포인터 초기화
    if (m_camera) {
        delete m_camera;
        m_camera = nullptr;
    }
    
    // 2. 동적 할당된 비디오 수신기 객체 메모리 해제 및 포인터 초기화
    if (m_video_surface) {
        delete m_video_surface;
        m_video_surface = nullptr;
    }
}
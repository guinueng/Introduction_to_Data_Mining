import sys
import subprocess
import time
import threading
from pathlib import Path

def realtime_output(proc, output_file):
    """실시간 출력을 처리하는 쓰레드 함수"""
    with open(output_file, 'a') as f:
        while True:
            line = proc.stdout.readline()
            if not line:
                break
            # 화면 출력 + 파일 기록 (플러시로 즉시 반영)
            print(line.strip())  # 실시간 화면 출력
            f.write(line)        # 파일 기록
            f.flush()

def run_elf_multiple_times():
    if len(sys.argv) != 3:
        print("Usage: python auto_eval.py <elf_path> <num_runs>")
        sys.exit(1)

    elf_path = Path(sys.argv[1])
    num_runs = int(sys.argv[2])

    if not elf_path.exists():
        print(f"Error: ELF file not found at {elf_path}")
        sys.exit(1)

    for i in range(1, num_runs + 1):
        output_file = f"1%_eps=2_minpts=5_{i}.txt"
        try:
            # 프로세스 시작
            proc = subprocess.Popen(
                ["stdbuf",
                 "-oL",
                 "/usr/bin/time",
                "-v",
                str(elf_path)],
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                bufsize=1,  # 라인 버퍼링 활성화
                universal_newlines=True
            )

            # 실시간 출력 쓰레드 시작
            output_thread = threading.Thread(
                target=realtime_output,
                args=(proc, output_file)
            )
            output_thread.start()

            # 주기적 상태 체크
            start_time = time.time()
            while proc.poll() is None:
                elapsed = time.time() - start_time
                if elapsed >= 30:
                    print(f"⏱️ Run {i}: Running for {int(elapsed//60)}m {int(elapsed%60)}s...")
                    start_time = time.time()  # 타이머 리셋
                time.sleep(1)  # CPU 사용량 줄이기

            output_thread.join()
            print(f"✅ Run {i} completed -> {output_file}")
            print("------------------------------------")

        except Exception as e:
            print(f"❌ Error in run {i}: {str(e)}")

if __name__ == "__main__":
    run_elf_multiple_times()

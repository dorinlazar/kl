#include "klprocess.hpp"
#include "klfs.hpp"
#include <unistd.h>
#include <sys/wait.h>

namespace kl {

char* klprocess_to_c_string(const Text& t) {
  char* p = static_cast<char*>(malloc(t.size() + 1));
  std::copy(t.begin(), t.end(), p);
  p[t.size()] = 0;
  return p;
}

void klprocess_free_c_string(char* p) { free(p); }

char** klprocess_to_c_array(const List<Text>& l) {
  char** res = static_cast<char**>(malloc(sizeof(char*) * (l.size() + 1)));
  char** current = res;
  for (const auto& t: l) {
    *current = klprocess_to_c_string(t);
    current++;
  }
  *current = nullptr;
  return res;
}

void free_c_array(char** arr) {
  char** current = arr;
  while (*current != nullptr) {
    free(*current);
    *current = nullptr;
    current++;
  }
  free(arr);
}

struct Process::Impl {
  pid_t m_pid = 0;
  bool m_started = false;
  char* m_exe;
  char** m_params;

public:
  explicit Impl(const List<Text>& params) {
    check(params.size() > 0, "Expected more than one parameter to Process invocation");
    m_exe = klprocess_to_c_string(FileSystem::executable_path(params[0]));
    m_params = klprocess_to_c_array(params);
  }
  Impl(const Impl&) = delete;
  Impl(Impl&&) = delete;
  Impl& operator=(const Impl&) = delete;
  Impl& operator=(Impl&&) = delete;

  ~Impl() {
    if (m_pid != 0) {
      join();
    }
    klprocess_free_c_string(m_exe);
    free_c_array(m_params);
  }

  void spawn() {
    m_pid = fork();
    if (m_pid == 0) {
      execve(m_exe, m_params, environ);
      _exit(1);
    }
  }

  int join() const {
    if (m_pid == 0) {
      return -1;
    }
    int status = 0;
    auto pid = waitpid(m_pid, &status, 0);
    if (pid == m_pid && WIFEXITED(status)) {
      return WEXITSTATUS(status);
    }
    return -1;
  }

  void kill(Signal sig) const {
    if (m_pid != 0) {
      int s = 0;
      switch (sig) {
      case Signal::Interrupt: s = SIGINT; break;
      case Signal::Terminate: s = SIGTERM; break;
      case Signal::Kill: s = SIGKILL; break;
      }
      ::kill(m_pid, s);
    }
  }

  Process::State state() const {
    int status = 0;
    if (m_pid == 0) {
      return Process::State::NotStarted;
    }
    if (waitpid(m_pid, &status, WNOHANG) != 0) {
      return Process::State::Error;
    }
    if (WIFEXITED(status)) {
      return Process::State::Finished;
    }
    return Process::State::Running;
  }

  pid_t pid() const { return m_pid; }
};

Process::Process(const List<Text>& params) { m_handler = std::make_unique<Impl>(params); }
Process::~Process() = default;
void Process::spawn() { m_handler->spawn(); }
int Process::join() { return m_handler->join(); }
void Process::kill(Signal s) { m_handler->kill(s); }

struct ExecutionMonitorNode {
  ExecutionNode* node;
  Process::Impl process;

public:
  explicit ExecutionMonitorNode(ExecutionNode* n) : node(n), process(node->m_params) {}
};

ExecutionNode* ProcessHorde::add_node(const List<Text>& params, const List<ExecutionNode*>& deps) {
  m_nodes.add(std::make_unique<ExecutionNode>(params, deps));
  auto p = m_nodes[m_nodes.size() - 1].get();
  if (deps.size() == 0) {
    m_execution_queue.push_back(p);
  } else {
    m_waiting_queue.add(p);
  }
  return p;
}

bool ProcessHorde::run(uint32_t n_jobs, bool verbose) {
  if (n_jobs < 1) {
    n_jobs = 1;
  }
  Dict<pid_t, ptr<ExecutionMonitorNode>> monitor;
  while (monitor.size() > 0 || !m_execution_queue.empty()) {
    while (monitor.size() < n_jobs && !m_execution_queue.empty()) {
      auto node = m_execution_queue.front();
      m_execution_queue.pop_front();
      auto monitor_node = std::make_shared<ExecutionMonitorNode>(node);
      if (verbose) {
        kl::log("> [{}]", kl::TextChain(node->m_params).join(','));
      }
      monitor_node->process.spawn();
      monitor.add(monitor_node->process.pid(), std::move(monitor_node));
    }

    int status = 0;
    auto pid = waitpid(-1, &status, 0);
    if (!monitor.has(pid)) {
      continue;
    }

    auto mon_node = monitor[pid];
    monitor.remove(pid);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
      mon_node->node->m_state = Process::State::Finished;

      size_t i = 0;
      while (i < m_waiting_queue.size()) {
        auto task = m_waiting_queue[i];
        if (task->m_dependencies.all(
                [](const kl::ExecutionNode* n) { return n->m_state == Process::State::Finished; })) {
          m_waiting_queue.remove_at(i);
          m_execution_queue.push_back(task);
        } else {
          i++;
        }
      }
    } else {
      mon_node->node->m_state = Process::State::Error;
      return false;
    }
  }

  return true;
}
} // namespace kl

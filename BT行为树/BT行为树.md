
## 一、 安装与知识介绍

Groot安装：参考 [[ubuntu\]BehaviorTree.CPP_V3编译安装_樱花回忆的博客-CSDN博客](https://blog.csdn.net/weixin_44979075/article/details/126419733)，安装时报错可参考：[Groot行为树UI编辑器安装与问题解决（完美教程！）_行为树编辑器-CSDN博客](https://blog.csdn.net/Netceor/article/details/129483160)

Groot 使用：

- 按上述方法安装完后，正常来说 Groot 文件夹内的文件大致会是图片1中所示
- 随后到 “/build/devel/lib/groot” 下，用终端输入 ./Groot 即可打开 Groot
- 打开后具体使用可参照图片2、3

behaviortree_cpp_v3(BT树的依赖)安装：`sudo apt install ros-noetic-behaviortree-cpp-v3`

BT树介绍：参考[BehaviorTree 行为树详解 完整版 （参考官方文档）-CSDN博客](https://blog.csdn.net/WDHDNRYX/article/details/127991377)

BT 树构建的基本步骤：编写节点的cpp文件 -> 到Groot连接上各个节点 -> 编写注册节点的cpp文件 




## 二、 导库

编写树节点时导 `#include "behaviortree_cpp_v3/action_node.h"` ，注册树节点时导 `#include "behaviortree_cpp_v3/bt_factory.h"`
当需要在BT树中使用服务通信或action通信时，则可直接包含 `bt_service_node.h` 或 `bt_action_node.h` 



## 三、 节点类型

### 1. sequence nodes 序列节点

- 在这类节点中，子节点返回 success，sequence node依次触发下一个子节点；子节点返回 failure，sequence node返回 failure
- Sequence 型：子节点返回 failure 则整个 Sequence 从头开始 (Restart)，子节点返回 running 则下次触发 Sequence 时从该子节点开始 (Tickagain)

- ReactiveSequence 型：子节点 failure 与 running 都是 Restart
- SequenceStar 型 ：子节点 failure 与 running 都是 Tickagain
- 当序列节点下面还有序列节点时，要注意两个序列节点的运行方式(如：一个 Sequence 下有一个 ReactiveSequence 和 SequenceStar，若 SequenceStar 返回了 failure，则这个 failure 会返回到 Sequence 上，导致下一次运行会从 ReactiveSequence 开始，而不是 SequenceStar)
### 2. Fallback nodes 选择节点

- 在这类节点中，在第一个子节点触发之前该类节点会置 running ，子节点 failure 则触发下一子节点，全部子节点 failure 则该节点返回 failure ；子节点 success 则不继续后面的子节点并返回 success
- Fallback 型：子节点 running 时 Tickagain
- ReactiveFallback 型：子节点 running 时 Restart

### 3. Decorators Nodes 装饰节点

- ==一个装饰器节点只有一个子节点==
- Inverter 型：反转子节点结果，但是子节点返回 running 时，同样返回 running
- ForceSuccess 型：子节点返回 running 时，同样返回 running，否则一律返回 success
- ForceFailure 型：子节点返回 running 时，同样返回 running，否则一律返回 failure
- Repeat 型：只要子节点返回 success，触发子节点至N次，若子节点返回 failure，中断循环并返回 failure
- RetryUntilSuccessful 型：只要子节点返回 failure，触发子节点至N次，若子节点返回 success，中断循环并返回 success

### 4.ActionNode行为节点

- 有状态异步操作 （StatefulAsyncAction）
  - 此模式在 **请求-答复模式**中特别有用，当操作向另一个进程发送异步请求时 ， 需要定期检查是否已收到回复，根据该回复它将返回成功或者失败
  - **StatefulAsyncAction** 的派生类必须重写以下虚拟方法 ， 而不是tick()。
    - **NodeStatus onStart（）**：当节点处于**空闲**状态时调用。 它可能会立即成功或失败，或者返回正在运行。在后一种情况下， 下次 tick 收到答复时，该方法将被执行。
    - **NodeStatus onRunning（）**：当节点处于 **RUNNING** 状态时调用。 返回新状态`onRunning`。
    - **void onHalted（）**：当该节点被树上的另一个节点中断时执行。、
    - 一般使用方法是在 onStart 中直接返回 running，然后再在 onRunning 中执行对应操作。
- 异步行为节点（AsyncActionNode）与同步行为节点（SyncActionNode）
  - 异步指节点可以返回 running，然后重新 tick，直到得到某个信号后返回 success 或 failure
  - 同步则是必须返回 success 或 failure 后才能进行下一次 tick



## 四、 xml 文件配置 BT 树

- 一般都是使用 Groot 来配置 BT 树，更加直观且方便

- 举例：

```xml
 <root main_tree_to_execute = "MainTree" >		<!-- 当树的节点只有一个时，main_tree_to_execute 可省略 -->
     <BehaviorTree ID="MainTree">
        <Sequence name="root_sequence">
            <SaySomething   name="action_hello" message="Hello"/>
            <!-- 下面这是输入输出端口 port 的形式，以键 “my_message”，寻找 blackboard 中对应的值 value -->
            <SaySomething message="{my_message}"/>		
            <OpenGripper    name="open_gripper"/>
            <ApproachObject name="approach_object"/>
            <CloseGripper   name="close_gripper"/>
        </Sequence>
     </BehaviorTree>
 </root>

```

- 上面的例子是紧凑模式的 xml ，但是使用 Groot 时需要清晰模式的 xml ，这时候需要加上以下代码：

```xml
<TreeNodeModel>
    <Action ID="SaySomething">
        <input_port name="message" type="std::string" />
    </Action>
    <Action ID="OpenGripper"/>
    <Action ID="ApproachObject"/>
    <Action ID="CloseGripper"/>      
</TreeNodeModel>
```

- 当需要加载另外的 xml 文件来配置树时，可以按照如下代码配置：

```xml
<root main_tree_to_execute = "MainTree">
    <include path="./subtree_A.xml" />		<!-- 子树 xml 文件的路径 -->
    <include path="./subtree_B.xml" />
    <BehaviorTree ID="MainTree">
        <Sequence>
            <SaySomething message="starting MainTree" />
            <SubTree ID="SubTreeA" />		<!-- 子树的 ID -->
            <SubTree ID="SubTreeB" />
        </Sequence>
    </BehaviorTree>
<root>

```



## 五、 创建 BT 树

### 1. 创建同步行为且无端口的树节点

- 推荐的方法是继承：

```c++
// 创建一个同步行为的树节点（无端口）
class ApproachObject : public BT::SyncActionNode
{
  public:
    ApproachObject(const std::string& name) :
        BT::SyncActionNode(name, {})
    {
    }

    // 必须重写函数 tick()，tick() 函数即为该节点需要进行的动作
    BT::NodeStatus tick() override
    {
        std::cout << "ApproachObject: " << this->name() << std::endl;
        return BT::NodeStatus::SUCCESS;
    }
};

```

- 也可以自定义树节点（不怎么用）：

```c++
using namespace BT;

// 利用函数指针创建的树节点
BT::NodeStatus CheckBattery()
{
    std::cout << "[ Battery: OK ]" << std::endl;
    return BT::NodeStatus::SUCCESS;
}

// 在自定义树节点中写入 open() 和 close() 函数
class GripperInterface
{
public:
    GripperInterface(): _open(true) {}		//将 GripperInterface 下面的一个名为 _open 的参数设置为 true

    NodeStatus open() {
        _open = true;
        std::cout << "GripperInterface::open" << std::endl;
        return NodeStatus::SUCCESS;
    }

    NodeStatus close() {
        std::cout << "GripperInterface::close" << std::endl;
        _open = false;
        return NodeStatus::SUCCESS;
    }

private:
    bool _open; // 可修改的参数
};

```



### 2. 创建同步行为且有端口的树节点

- 举例：

```c++
class SaySomething : public SyncActionNode
{
  public:
    // 相比无端口的节点多了个 config
    SaySomething(const std::string& name, const NodeConfiguration& config)
      : SyncActionNode(name, config)
    { }

    // 带有端口时必须有这一段代码
    static PortsList providedPorts()
    {
        return { InputPort<std::string>("message") };	//是输入端口，读取 blackboard 上的 message 键对应的值
    }
    
    NodeStatus tick() override
    {
        Optional<std::string> msg = getInput<std::string>("message");
        // 检查消息类型是否是指定的类型，不是的话报错
        if (!msg)
        {
            throw BT::RuntimeError("missing required input [message]: ", 
                                   msg.error() );
        }

        // 下面这句是对消息的值 msg.value() 进行操作
        std::cout << "Robot says: " << msg.value() << std::endl;
        return NodeStatus::SUCCESS;
    }
};
```

- 当有多个输入端口时，上面的一块代码可换为：

```c++
static PortsList providedPorts() {
    BT::PortsList ports_list;
    ports_list.insert(BT::InputPort<std::string>("aa_msg"));
    ports_list.insert(BT::InputPort<std::string>("bb_msg"));
    ports_list.insert(BT::InputPort<std::string>("cc_msg"));
    return ports_list;
  }
```

- 当端口是输出端口 (写入键值对) 时，上面两块代码应换为：

```c++
static PortsList providedPorts()
{
    return { OutputPort<std::string>("text") };			//输出到 blackboard 上的键是 text
}
NodeStatus tick() override
{
    setOutput("text", "The answer is 42" );				//设置 text 的值为 "The answer is 42"
    return NodeStatus::SUCCESS;
}
```

- 键值对的配置也可以在 xml 文件中完成：

```xml
 <SetBlackboard   output_key="the_answer" value="The answer is 42" />
```



### 3. 创建异步行为的树节点

- 只需要将同步行为的一部分代码按如下方式修改即可：

```c++
class MoveBaseAction : public AsyncActionNode
{
  public:
    MoveBaseAction(const std::string& name, const NodeConfiguration& config)
      : AsyncActionNode(name, config)
    { }
    
    //新增部分，用于暂停
    void halt() override
    {
        _halt_requested.store(true);
    }
   private:
    std::atomic_bool _halt_requested;
};
```



### 4. 自定义端口 port 的数据类型

- 需要链接到一个模板特例：

```c++
//自定义的数据类型
struct Position2D 
{ 
  double x;
  double y; 
};

//模板特例
namespace BT
{
    template <> inline Position2D convertFromString(StringView str)
    {
        // 以分号分隔各个数据
        auto parts = splitString(str, ';');
        if (parts.size() != 2)		//2是数据的个数
        {
            throw RuntimeError("invalid input)");
        }
        else{
            Position2D output;
            output.x     = convertFromString<double>(parts[0]);
            output.y     = convertFromString<double>(parts[1]);
            return output;
        }
    }
}

```

- 后面在使用时就可以写 `<Position2D>` 了



### 5. 父树和子树端口的映射

- 举例：

```xml
<root main_tree_to_execute = "MainTree">

    <BehaviorTree ID="MainTree">

        <Sequence name="main_sequence">
            <SetBlackboard output_key="move_goal" value="1;2;3" />
            <SubTree ID="MoveRobot" target="move_goal" output="move_result" />
            <SaySomething message="{move_result}"/>
        </Sequence>

    </BehaviorTree>

    <BehaviorTree ID="MoveRobot">
        <Fallback name="move_robot_main">
            <SequenceStar>
                <MoveBase       goal="{target}"/>
                <SetBlackboard output_key="output" value="mission accomplished" />
            </SequenceStar>
            <ForceFailure>
                <SetBlackboard output_key="output" value="mission failed" />
            </ForceFailure>
        </Fallback>
    </BehaviorTree>

</root>

```




### 6. 自定义节点的参数个数

- 举例：

```c++
class Action_A: public SyncActionNode
{
public:
    // 修改的部分，新增的参数个数及类型可以自行决定
    Action_A(const std::string& name, const NodeConfiguration& config,
             int arg1, double arg2, std::string arg3 ):
        SyncActionNode(name, config),
        _arg1(arg1),
        _arg2(arg2),
        _arg3(arg3) {}

    static PortsList providedPorts() { return {}; }
    NodeStatus tick() override;

private:
    int _arg1;
    double _arg2;
    std::string _arg3;
};

```



### 7. 注册节点

- 举例：

```c++
#include "behaviortree_cpp_v3/bt_factory.h"
#include "dummy_nodes.h"

int main()
{
    BehaviorTreeFactory factory;
    using namespace DummyNodes;

    // 推荐的注册方式是继承
    factory.registerNodeType<ApproachObject>("ApproachObject");

    //也可以用函数指针注册
    factory.registerSimpleCondition("CheckBattery", std::bind(CheckBattery));

    //也可以自定义节点注册
    GripperInterface gripper;
    factory.registerSimpleAction("OpenGripper", 
                                 std::bind(&GripperInterface::open, &gripper));
    factory.registerSimpleAction("CloseGripper", 
                                 std::bind(&GripperInterface::close, &gripper));

    //加载 xml 文件
    auto tree = factory.createTreeFromFile("./my_tree.xml");
	
    //启动节点需要 tick
    tree.tickRoot();

    return 0;
}

```

- 当节点的参数是自定义时，应该这样注册：

```c++
BehaviorTreeFactory factory;

NodeBuilder builder_A =
   [](const std::string& name, const NodeConfiguration& config)
{
    return std::make_unique<Action_A>( name, config, 42, 3.14, "hello world" );
};

factory.registerBuilder<Action_A>( "Action_A", builder_A);

```

- 在注册节点时，可顺带将树的状态进行打印，便于调试时观察树的运行状态，具体操作参考文件 `control_loop.h` 和 `control_loop.cpp`


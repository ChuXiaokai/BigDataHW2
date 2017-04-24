#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <set>
#include <list>

#include "GraphLite.h"
using namespace std;
typedef list<int64_t> ListArr;

#define VERTEX_CLASS_NAME(name) DirectedTriangleCount##name

typedef struct GraphNode {  // 定义四种三角形的结构
    int in;//in triangle
    int out;//out triangle
    int through;//through triangle
    int cycle;//cycle triangle
} GraphNode;

int len1=sizeof(GraphNode);
int len2=sizeof(double);
int len3=sizeof(int64_t);

class VERTEX_CLASS_NAME(InputFormatter): public InputFormatter {
public:
    int64_t getVertexNum() {
        unsigned long long n;
        sscanf(m_ptotal_vertex_line, "%lld", &n);
        m_total_vertex = n;
        return m_total_vertex;
    }

    int64_t getEdgeNum() {
        unsigned long long n;
        sscanf(m_ptotal_edge_line, "%lld", &n);
        m_total_edge = n;
        return m_total_edge;
    }

    int getVertexValueSize() {  // 获取图的节点数目？
    //    m_n_value_size = sizeof(GraphNode);
	m_n_value_size = len1;
        return m_n_value_size;
    }

    int getEdgeValueSize() {
        // m_e_value_size = sizeof(double);  // 啥意思？
        m_e_value_size = len2;
        return m_e_value_size;
    }

    int getMessageValueSize() {
        // _m_value_size = sizeof(ValueType);
        m_m_value_size = len3;  // 啥意思？
        return m_m_value_size;
    }

    void loadGraph() {
        unsigned long long last_vertex;
        unsigned long long from;
        unsigned long long to;
        double weight = 0;
	int outdegree = 0;
        const char *line = getEdgeLine();
	GraphNode value={0,0,0,0};  // 定义三角形的个数，全局先初始化为0
        sscanf(line, "%lld %lld", &from, &to);
        addEdge(from, to, &weight);

        last_vertex = from;
        ++outdegree;
        for (int64_t i = 1; i < m_total_edge; ++i) {
            line = getEdgeLine();

            sscanf(line, "%lld %lld", &from, &to);
            if (last_vertex != from) {
                addVertex(last_vertex, &value, outdegree);
                last_vertex = from;
                outdegree = 1;
            } else {
                ++outdegree;
            }
            addEdge(from, to, &weight);
        }
        addVertex(last_vertex, &value, outdegree);
    }
};

class VERTEX_CLASS_NAME(OutputFormatter): public OutputFormatter {
public:
    void writeResult() {
        int64_t v;
	GraphNode value;
        char s[1024];
	int number;
	ResultIterator a; // OutputFormatter.h => get the result after computation.
        
        a.getIdValue(v, &value);  // 获取每个节点的数值
        number = sprintf(s, "in triangle: %d\nout triangle: %d\nthrough triangle: %d\ncycle triangle: %d\n", value.in, value.out, value.through, value.cycle);
        writeNextResLine(s, number);
    }
};
//*Aggregator
class VERTEX_CLASS_NAME(Aggregator): public Aggregator<GraphNode> {
public:
    void init() {
        // 给m_global和local开出一个GraphNode大小的内存
        memset(&m_global, 0 , len1); 
        memset(&m_local, 0 , len1);
    }

    void* getGlobal() { 
	return &m_global; 
    }

    void setGlobal(const void* qs) { 
	memcpy(&m_global, qs, sizeof(GraphNode)); 
    }

    void* getLocal() { 
	return &m_local;  
    }

    void merge(const void* qs) {   // 啥意思？
	m_global.in = m_global.in +((GraphNode*)qs)->in;
	m_global.out = m_global.out+((GraphNode*)qs)->out;
        m_global.through =m_global.through + ((GraphNode*)qs)->through;
        m_global.cycle =m_global.cycle + ((GraphNode*)qs)->cycle;
    }
    void accumulate(const void* qs) {   // ??
        m_local.in = m_local.in +((GraphNode*)qs)->in;
	m_local.out = m_local.out+((GraphNode*)qs)->out;
        m_local.through =m_local.through + ((GraphNode*)qs)->through;
        m_local.cycle =m_local.cycle + ((GraphNode*)qs)->cycle;
    }
};
//compute
class VERTEX_CLASS_NAME(): public Vertex <GraphNode, double, int64_t> {
public:
    void compute(MessageIterator* pmsgs) {
	switch(getSuperstep())//or getSuperstep=()
	{
        case 0:
	{
		sendMessageToAllNeighbors(m_pme->m_v_id); // send the node's id to its neighbour as the out-neighbour
		break;
	}
        case 1:
	{
                for(; !pmsgs->done(); pmsgs->next()) 
		{
		sendMessageTo(m_pme->m_v_id, pmsgs->getValue()); // send to the node itself
                sendMessageToAllNeighbors(pmsgs->getValue());    // send in-neighbout              
        	}
		break;
	}
        case 2:
	{
            ListArr prepre_neigh;
            set<int64_t> in_neigh; // 孩子节点
	    set<int64_t> out_neigh;
	    auto t=prepre_neigh.begin();
            GraphNode GraphNode_local = {0,0,0,0};
	
	    for(auto ac = getOutEdgeIterator(); !ac.done(); ac.next())
                out_neigh.insert(ac.target());

            for(; !pmsgs->done(); pmsgs->next())
                if(((Msg*)pmsgs->getCurrent())->s_id == m_pme->m_v_id)
                    in_neigh.insert(pmsgs->getValue());           // form in_set
                else prepre_neigh.push_front(pmsgs->getValue());  // form pre_in_list
                                         
//	    while(t!=prepre_neigh.begin())
		for(auto t = prepre_neigh.begin();t!=prepre_neigh.end(); ++t)
	    {
                if(in_neigh.find(*t) != in_neigh.end()) {
			GraphNode_local.through=GraphNode_local.through+1;
			GraphNode_local.in=GraphNode_local.through;
			GraphNode_local.out=GraphNode_local.in;
		    }
                if(out_neigh.find(*t) != out_neigh.end()) 
			GraphNode_local.cycle=GraphNode_local.cycle+1;
	//	t=t-1;
        }
            accumulateAggr(0, &GraphNode_local);
	    break;
        } 
	case 3:
	{
            * mutableValue() = *(GraphNode*)getAggrGlobal(0);
            voteToHalt();
	    break;
        }
    }
};

class VERTEX_CLASS_NAME(Graph): public Graph {
public:
    VERTEX_CLASS_NAME(Aggregator)* aggregator;

public:
    void init(int argc, char* argv[]) {

        setNumHosts(5);
        setHost(0, "localhost", 1411);
        setHost(1, "localhost", 1421);
        setHost(2, "localhost", 1431);
        setHost(3, "localhost", 1441);
        setHost(4, "localhost", 1451);

        if (argc < 3) {
           printf ("Usage: %s <input path> <output path>\n", argv[0]);
           exit(1);
        }

        m_pin_path = argv[1];
        m_pout_path = argv[2];

        aggregator = new VERTEX_CLASS_NAME(Aggregator)[1];
        regNumAggr(1);
        regAggr(0, &aggregator[0]);
    }

    void term() {
        delete[] aggregator;
    }
};

/* STOP: do not change the code below. */
extern "C" Graph* create_graph() {
    Graph* pgraph = new VERTEX_CLASS_NAME(Graph);

    pgraph->m_pin_formatter = new VERTEX_CLASS_NAME(InputFormatter);
    pgraph->m_pout_formatter = new VERTEX_CLASS_NAME(OutputFormatter);
    pgraph->m_pver_base = new VERTEX_CLASS_NAME();

    return pgraph;
}

extern "C" void destroy_graph(Graph* pobject) {
    delete ( VERTEX_CLASS_NAME()* )(pobject->m_pver_base);
    delete ( VERTEX_CLASS_NAME(OutputFormatter)* )(pobject->m_pout_formatter);
    delete ( VERTEX_CLASS_NAME(InputFormatter)* )(pobject->m_pin_formatter);
    delete ( VERTEX_CLASS_NAME(Graph)* )pobject;
}

	
	

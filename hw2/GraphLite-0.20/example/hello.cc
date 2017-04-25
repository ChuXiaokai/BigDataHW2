#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <set>

#include "GraphLite.h"

#define VERTEX_CLASS_NAME(name) DirectedTriangleCount##name


typedef struct Triangle {
    int in, out, through, cycle;
} Triangle;

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
    int getVertexValueSize() {
        m_n_value_size = sizeof(Triangle);
        return m_n_value_size;
    }
    int getEdgeValueSize() {
        m_e_value_size = sizeof(double);
        return m_e_value_size;
    }
    int getMessageValueSize() {
        m_m_value_size = sizeof(int64_t);
        return m_m_value_size;
    }
    void loadGraph() {
        unsigned long long last_vertex;
        unsigned long long from;
        unsigned long long to;
        double weight = 0;

        Triangle value;
        int outdegree = 0;

        const char *line = getEdgeLine();

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
        int64_t vid;
        char s[1024];
        Triangle value;

        ResultIterator r_iter;
        r_iter.getIdValue(vid, &value);
        int n = sprintf(s, "in: %d\nout: %d\nthrough: %d\ncycle: %d\n", value.through, value.through, value.through, value.cycle);
        writeNextResLine(s, n);
    }
};

class VERTEX_CLASS_NAME(Aggregator): public Aggregator<Triangle> {
public:
    void init() {
        // every time when a step finished. m_global reset to 0
        memset(&m_global, 0, sizeof(Triangle));
        memset(&m_local, 0, sizeof(Triangle));
    }
    void* getGlobal() {  // when merge finished. every worker do getGlobal
        printf("get getGlobal() global.through = %d, global.cycle = %d\n", m_global.through, m_global.cycle);
        return &m_global; 
    }
    void setGlobal(const void* p) {  // worker do. 
        printf("set getGlobal !!!\n");
        memmove(&m_global, p, sizeof(Triangle)); 
    }

    void* getLocal() {  
        printf("getlocal() m_local.through = %d\n", m_local.through);
        return &m_local; 
    }

    void merge(const void* p) {  // master
                                // every time a worker push its work. master do merge
        printf("merge() m_global.through = %d, p->through = %d\n", m_global.through, ((Triangle*)p)->through);
        m_global.through += ((Triangle*)p)->through;
        m_global.cycle += ((Triangle*)p)->cycle;
        m_global.in += ((Triangle*)p)->in;
        m_global.out += ((Triangle*)p)->out;

    }
    void accumulate(const void* p) {  // every vertex
        printf("accumulate() m_local.through = %d, p->through = %d\n", m_local.through, ((Triangle*)p)->through);
        m_local.through += ((Triangle*)p)->through;
        m_local.cycle += ((Triangle*)p)->cycle;
        m_local.in += ((Triangle*)p)->in;
        m_local.out += ((Triangle*)p)->out;
    }
};

class VERTEX_CLASS_NAME(): public Vertex <Triangle, double, int64_t> {
public:
    void compute(MessageIterator* pmsgs) {
        if(getSuperstep() == 0){
            sendMessageToAllNeighbors(getVertexId()); // send outedge's src node info to neighbor
            printf("================== step 0 =================\n");
        }
        else if(getSuperstep() == 1){
            printf("================== step 1 =================\n");
            for(; !pmsgs->done(); pmsgs->next()) {
                sendMessageToAllNeighbors(pmsgs->getValue());    // send inedge's src node info to neighbor
                // sendMessageTo(m_pme->m_v_id, pmsgs->getValue()); // tell superstep 2 inedge info
                sendMessageTo(getVertexId(), pmsgs->getValue()); // tell superstep 2 inedge info
            }
        }
        else if(getSuperstep() == 2) {
            printf("================== step 2 =================\n");
            set<int64_t> l_node;  // last_node
            set<int64_t> n_node;  // next_node
            vector<int64_t> ll_node;  // last last node
            Triangle tmp;
            tmp.through = 0;
            tmp.in = 0;
            tmp.out = 0;
            tmp.cycle =0;
            for(; !pmsgs->done(); pmsgs->next())
                if(((Msg*)pmsgs->getCurrent())->s_id == getVertexId()) {   // s_id => source id of the msg; if this message is send to itself
                    l_node.insert(pmsgs->getValue());           // get all its l_node in set<l_node>
                }
                else {
                    ll_node.push_back(pmsgs->getValue());  // establish ll_node vector
                }

            for(auto iter = getOutEdgeIterator(); !iter.done(); iter.next()) { // count the out edge 1->2
                n_node.insert(iter.target());                 // find all the n_node ; target 1->2  target = 2
            }
            for(auto iter = ll_node.begin(); iter != ll_node.end(); iter++) {  // find cycle
                if(n_node.find(*iter) != n_node.end()) {
                    tmp.cycle += 1;
                }
            }
            for(auto iter = ll_node.begin(); iter != ll_node.end(); iter++) {  // find others
                if(l_node.find(*iter) != l_node.end()){
                    tmp.through += 1;  // it means l_node has a iter 
                    tmp.in += 1;
                    tmp.out += 1;
                }
            }
            // accu
            accumulateAggr(0, &tmp);
        } else  {
            printf("================== step 3 =================\n");
            * mutableValue() = *(Triangle*)getAggrGlobal(0);
            voteToHalt();
        }
    }
};

class VERTEX_CLASS_NAME(Graph): public Graph {
public:
    VERTEX_CLASS_NAME(Aggregator)* aggregator;

public:
    // argv[0]: DirectedTriangleCount.so
    // argv[1]: <input path>
    // argv[2]: <output path>
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

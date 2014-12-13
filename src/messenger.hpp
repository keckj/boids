#ifndef MESSENGER_HPP
#define MESSENGER_HPP

#include <mpi.h>
#include <vector>
#include "options.hpp"
#include "boid/agent.hpp"
#include "utils/types.hpp"


class Messenger {

    public:

        /*
         * comm : Current communicator
         * opt : Options to broadcast
         * root : process that is the source of the broacast
         */
        inline void broadcastOptions(MPI_Comm comm, Options *opt, int root) {
            MPI_Bcast(opt, sizeof(Options)/sizeof(double), MPI_DOUBLE, 0, comm);
        }


        /*
         * comm : Current communicator
         * agents : Container to which to append received boids
         * agentsForRanks : Maps Containers of boids to send to ranks
         * sourceRanks : ranks from which to receive boids (typically the keys of agentsForRanks)
         */
        inline void exchangeAgents(MPI_Comm comm, Container &agents, 
                                   std::map<int, Container> &agentsForRanks, 
                                   std::vector<int> &sourceRanks) 
        {
            sendAgents(comm, agentsForRanks);
            receiveAgents(comm, agents, sourceRanks);
            waitForSendCompletion();
            MPI_Barrier(comm);
        }
        
        /*
         * comm : Current communicator
         * receivedMeanBoids : Container that will contain received boids
         * meanBoidToSend : Mean boid to send
         * sourceRanks : ranks with which to exchange mean boids
         */
        inline void exchangeMeanBoids(MPI_Comm comm, Container &receivedMeanBoids, 
                                 Agent &meanBoidToSend, 
                                 std::vector<int> &sourceRanks) 
        {
            sendMeanBoid(comm, meanBoidToSend, sourceRanks);
            receiveMeanBoids(comm, receivedMeanBoids, sourceRanks);
            waitForSendCompletion();
            MPI_Barrier(comm);
        }

    private:

        inline void sendAgents(MPI_Comm comm, std::map<int, Container> &agentsForRanks) {
            MPI_Request req;

            // Send everything immediately to prevent deadlocks 
            for (auto it : agentsForRanks) {
                int targetRank = it.first;
                Container agents = it.second;
                int tag = 0;

                MPI_Isend(&agents[0], agents.size()*realsPerAgent, MPI_DOUBLE, targetRank, tag, comm, &req);
                pendingRequests.push_back(req);
            }
        }
           
        inline void sendMeanBoid(MPI_Comm comm, Agent &meanBoidToSend, std::vector<int> &sourceRanks) {
            MPI_Request req;
            int tag = 0;
                
            for (int rank : sourceRanks) {
                MPI_Isend(&meanBoidToSend, realsPerAgent, MPI_DOUBLE, rank, tag, comm, &req);
                pendingRequests.push_back(req);
            }
        }

        // Note: this method appends the received agents to the container
        inline void receiveAgents(MPI_Comm comm, Container &agents, std::vector<int> &sourceRanks) {
            MPI_Status stat;
            int nAgents = 0;
            int sizes[sourceRanks.size()];
            
            // Wait until everything arrives
            int i = 0;
            for (int sourceRank : sourceRanks) {
                MPI_Probe(sourceRank, MPI_ANY_TAG, comm, &stat);
                MPI_Get_count(&stat, MPI_DOUBLE, &sizes[i]);
                nAgents += sizes[i] / realsPerAgent;
                i++;
            }

            // Make room for copying
            agents.reserve(agents.size() + nAgents);

            // Concatenate the Containers
            int pos = agents.size();
            i = 0;
            for (int sourceRank : sourceRanks) {
                MPI_Recv(&agents[pos], sizes[i], MPI_DOUBLE, sourceRank, MPI_ANY_TAG, comm, &stat);
                pos += sizes[i] / realsPerAgent;
                i++;
            }
        }
            
        inline void receiveMeanBoids(MPI_Comm comm, Container &receivedMeanBoids, std::vector<int> &sourceRanks) {
            // Clear and reserve space in the container
            receivedMeanBoids.clear();
            receivedMeanBoids.reserve(sourceRanks.size());
            
            int pos = 0;
            for (int sourceRank : sourceRanks) {
                MPI_Recv(&receivedMeanBoids[pos], realsPerAgent, MPI_DOUBLE, sourceRank, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
                pos++;
            }
        }

        inline void waitForSendCompletion() {
            for (MPI_Request req : pendingRequests) {    
                MPI_Wait(&req, MPI_STATUS_IGNORE);
            }
            pendingRequests.clear();
        }

        /*static inline void exchangeAgents(MPI_Comm comm, Container &agentsToSend, Container &agentsToReceive, int targetRank) {
            MPI_Request req;
            MPI_Status statSend, statRecv;
            int tagSend = 0, tagReceive = 0;
            int recvBufferSize;

            MPI_Isend(&agentsToSend[0], agentsToSend.size()*realsPerAgent, MPI_DOUBLE, targetRank, tagSend, comm, &req);
            MPI_Probe(targetRank, tagReceive, comm, &statRecv);
            MPI_Get_count(&statRecv, MPI_DOUBLE, &recvBufferSize);
            agentsToReceive.reserve(recvBufferSize/realsPerAgent);
            MPI_Recv(&agentsToReceive[0], recvBufferSize, MPI_DOUBLE, targetRank, tagReceive, comm, &statRecv);
            MPI_Wait(&req, &statSend);
        }

        static inline int getRankFromDirection(MPI_Comm comm, int localRank, int direction[3]) {
            int currentRank = localRank;

            // Since MPI_Cart_shift can only shift in 1 direction, we're doing one shift per direction
            if (direction[0] != 0)
                MPI_Cart_shift(comm, 0, direction[0], &currentRank, &currentRank);
            if (direction[1] != 0)
                MPI_Cart_shift(comm, 1, direction[1], &currentRank, &currentRank);
            if (direction[2] != 0)
                MPI_Cart_shift(comm, 2, direction[2], &currentRank, &currentRank);
            return currentRank;
        }*/

        std::vector<MPI_Request> pendingRequests;
        static const int realsPerAgent = sizeof(Agent)/sizeof(Real);
};

#endif
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kohei <kohei@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 18:03:37 by kohei             #+#    #+#             */
/*   Updated: 2023/06/23 19:35:50 by kohei            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <vector>
# include <poll.h>

class ServerMonitor;
class Server;
class Request;
class Response;

class Client
{
    public:
        Client(void);
        ~Client(void);
        Client(const Client & client);
        Client &operator=(const Client &client);

        void                            setRequests(Request *request);
        void                            setResponses(Response *responses);
        void                            setPollfd(pollfd fd);
        void                            setEvent(short event);
        void                            unsetEvent(short event);

        const std::vector<Request *>    &getRequests(void) const;
        const std::vector<Response *>   &getResponses(void) const;
        const pollfd                    &getPollfd(void) const;
        
        void							addRequest(const std::string &msg);
	    void							addResponse(const Server &servconf);

        void                            checkPendingRequests(ServerMonitor& serverMonitor);
        void							takeMessagesFromClient(ServerMonitor& serverMonitor);
        void							sendMessagesToClient();

    private:
        std::string                     getMsg();
        std::vector<Request *> requests_;
        std::vector<Response *> responses_;
        pollfd                  pollfd_;
};

#endif

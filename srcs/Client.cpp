/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kohei <kohei@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 22:14:38 by kohei             #+#    #+#             */
/*   Updated: 2023/06/23 20:31:28 by kohei            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <netinet/in.h>
#include "ServerMonitor.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <iostream>

Client::Client(void): requests_(), responses_(), pollfd_()
{
    
}

Client::~Client(void)
{
    
}

Client::Client(const Client &client) : requests_(client.requests_),
                                        responses_(client.responses_),
                                        pollfd_(client.pollfd_) {

}

Client &Client::operator=(const Client &client)
{
    if (this != &client)
    {
        this->requests_ = client.getRequests();
        this->responses_ = client.getResponses();
        this->pollfd_ = client.getPollfd();
    }
    return (*this);
}

void  Client::setRequests(Request *request)
{
    this->requests_.push_back(request);
}

void  Client::setResponses(Response *response)
{
    this->responses_.push_back(response);
}

void  Client::setPollfd(pollfd fd)
{
    this->pollfd_ = fd;
}

void    Client::setEvent(short event)
{
    this->pollfd_.events |= event;
}

void    Client::unsetEvent(short event)
{
    this->pollfd_.events &= ~event;
}

const std::vector<Request *> &Client::getRequests(void) const
{
    return (this->requests_);
}

const std::vector<Response *> &Client::getResponses(void) const
{
    return (this->responses_);
}

const pollfd  &Client::getPollfd(void) const
{
    return (this->pollfd_);
}

void	Client::takeMessagesFromClient(ServerMonitor& serverMonitor) {
    pollfd clientPoll = this->getPollfd();
    if (clientPoll.revents & POLLIN) {
        std::string msg = this->getMsg();
        if (msg.empty()) {
            return ;
        }
        std::cout << msg << std::endl;
        Request *req = createRequestFromRequestString(msg);
        Response *res = req->execute(serverMonitor);
        if (res) {
            this->responses_.push_back(res);
            this->setEvent(POLLOUT);
            delete req;
        } else {
            this->requests_.push_back(req);
        }
    }
}

void	Client::sendMessagesToClient(void) {
    pollfd clientPoll = this->getPollfd();
    if (clientPoll.revents & POLLOUT) {
        std::vector<Response *>::iterator it = responses_.begin();
        std::vector<Response *>::iterator ite = responses_.end();
        for (; it != ite; it++)
        {
            std::string resMsg = (*it)->getRawMessage();
            int BytesSent = send(pollfd_.fd, resMsg.c_str(), resMsg.length(), 0);
            if (BytesSent < 0)
                throw std::runtime_error("send error");
            delete *it;
            this->responses_.erase(it);
        }
        this->unsetEvent(POLLOUT);
    }
}

std::string Client::getMsg(void) {

	char		buff[BUFFER_SIZE + 1];
	std::string	msg;
    int         socketFd = this->getPollfd().fd;

	while (true) {
		int	bytesRead = recv(socketFd, buff, BUFFER_SIZE, 0);
		buff[bytesRead] = '\0';
		msg += buff;
		if (bytesRead < 0) {
			throw std::runtime_error("An issue occured while receiving message");
		} else if (bytesRead == 0) {
			throw std::runtime_error("End of communication");
		} else if (bytesRead < BUFFER_SIZE) {
			break ;
		}
	}
	return msg;
}

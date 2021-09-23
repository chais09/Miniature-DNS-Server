# Miniature-DNS-Server
<!-- ALL-CONTRIBUTORS-BADGE:START - Do not remove or modify this section -->
[![All Contributors](https://img.shields.io/badge/all_contributors-1-orange.svg?style=flat-square)](#contributors-)
<!-- ALL-CONTRIBUTORS-BADGE:END -->

The Domain Name System (DNS) provides, among other things, the mapping between human-meaningful hostnames like lms.unimelb.edu.au and the numeric IP addresses that indicate where packets should be sent. DNS consists of a hierarchy of servers, each knowing a portion of the complete mapping. In this project, you will write a DNS server that accepts requests for IPv6 addresses and serves them either from its own cache or by querying servers higher up the hierarchy. Each transaction consists of atmost four messages: one from your client to you, one from you to your upstream server, one from your upstream server to you and one from you to your client. The middle two can be sometimes skipped if you cache some of the answers.

## Contributors ✨

Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tr>
    <td align="center"><a href="https://chais09.github.io"><img src="https://avatars.githubusercontent.com/u/55682227?v=4?s=100" width="100px;" alt=""/><br /><sub><b>chais09</b></sub></a><br /><a href="#projectManagement-chais09" title="Project Management">📆</a> <a href="https://github.com/chais09/Miniature-DNS-Server/commits?author=chais09" title="Code">💻</a></td>
  </tr>
</table>

<!-- markdownlint-restore -->
<!-- prettier-ignore-end -->

<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!
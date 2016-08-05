<?php

namespace Drupal\statsd\Logger;

use Drupal\Core\Logger\RfcLoggerTrait;
use Psr\Log\LoggerInterface;
use Drupal\Core\Config\ConfigFactoryInterface;

class StatsDLogger implements LoggerInterface {
  use RfcLoggerTrait;

  protected $config;

  /**
   * Construct a StatsDLogger interface to allow log event response.
   */
  public function __construct(ConfigFactoryInterface $config) {
    $this->config = $config;
  }

  /**
   * @inheritdoc
   */
  public function log($level, $message, array $context = array()) {

    if (strstr($message, 'Login attempt failed for')) {
      statsd_user_login_failed($entry['user']);
    }

    $enabled = $this->config->get('events.watchdog_events');
    $level   = $this->config->get('events.watchdog_level');

    if (!$enabled || $level < $entry['severity']) {
      return;
    }

    $levels = watchdog_severity_levels();
    $data   = array(
      'watchdog.type.' . $entry['type'],
      'watchdog.severity.' . $levels[$entry['severity']],
    );

    statsd_call($data);
  }

}
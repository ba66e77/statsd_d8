<?php

namespace Drupal\statsd\Logger;

use Drupal\Core\Logger\RfcLoggerTrait;
use Drupal\Core\Logger\RfcLogLevel;
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

    $enabled = $this->config->get('events.watchdog_events');
    $eventThreshold   = $this->config->get('events.watchdog_level');

    if (!$enabled || $eventThreshold < $level) {
      return;
    }

    if (strstr($message, 'Login attempt failed for')) {
      statsd_user_login_failed($context['user']);
    }

    $levels = RfcLogLevel::getLevels();
    $data   = array(
      'watchdog.type.' . $context['type'],
      'watchdog.severity.' . $levels[$level],
    );

    statsd_call($data);
  }

}